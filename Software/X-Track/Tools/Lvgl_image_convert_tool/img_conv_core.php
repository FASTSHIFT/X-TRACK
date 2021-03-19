<?php

class Converter {
    var $dith;      /*Dithering enable/disable*/
    var $w;         /*Image width*/
    var $h;         /*Image height*/
    var $cf;        /*Color format*/
    var $alpha;     /*Add alpha byte or not*/
    var $chroma;    /*Chroma keyed?*/
    var $d_out;     /*Output data (result)*/
    var $img;       /*Image resource*/
    var $out_name;  /*Name of the output file*/
	var $output_dir;/*Dir of the output file*/
    var $path;      /*Path to the image file*/

    /*Helper variables*/
    var $r_act;
    var $b_act;
    var $g_act;

    /*For dithering*/
    var $r_earr;  /*Classification error for next row of pixels*/
    var $g_earr;
    var $b_earr;

    var $r_nerr;  /*Classification error for next pixel*/
    var $g_err;
    var $b_nerr;

    const CF_TRUE_COLOR_332 = 0;      /*Helper formats. Used internally*/
    const CF_TRUE_COLOR_565 = 1;
    const CF_TRUE_COLOR_565_SWAP = 2;
    const CF_TRUE_COLOR_888 = 3;
    const CF_ALPHA_1_BIT = 4;
    const CF_ALPHA_2_BIT = 5;
    const CF_ALPHA_4_BIT = 6;
    const CF_ALPHA_8_BIT = 7;
    const CF_INDEXED_1_BIT = 8;
    const CF_INDEXED_2_BIT = 9;
    const CF_INDEXED_4_BIT = 10;
    const CF_INDEXED_8_BIT = 11;
    const CF_RAW = 12;
    const CF_RAW_ALPHA = 13;
    const CF_RAW_CHROMA = 12;

    const CF_TRUE_COLOR = 100;          /*Helper formats is C arrays contains all treu color formats (usin in "download")*/
    const CF_TRUE_COLOR_ALPHA = 101;
    const CF_TRUE_COLOR_CHROMA = 102;

    /**
     * @param path the image file
     * @param real_name the real image file name (different in online mode)
     * @param out_name name of the output file
     * @param dith 1: dither enabled; 0: disabled
     * @param cf color format
     */
    function __construct ($path, $real_name,$output, $out_name ,$dith) {
        $this->dith = $dith;
        $this->out_name = $out_name;
        $this->path = $path;
		$this->output_dir = $output;

        $size = getimagesize($path);
        $this->w = $size[0];
        $this->h = $size[1];

        $ext = pathinfo($real_name, PATHINFO_EXTENSION);
        if(!strcmp(strtolower($ext), "png")) $this->img = imagecreatefrompng($path);
        else if(!strcmp(strtolower($ext), "bmp")) $this->img = imagecreatefrombmp($path);
        else if(!strcmp(strtolower($ext), "jpg")) $this->img = imagecreatefromjpeg($path);
        else if(!strcmp(strtolower($ext), "jpeg")) $this->img = imagecreatefromjpeg($path);
        else {
            echo("$ext is a not supported image type. use png, jpg, jpeg or bmp");
            exit(1);
        }

        $this->r_earr = array();  /*Classification error for next row of pixels*/
        $this->g_earr = array();
        $this->b_earr = array();

        if($this->dith) {
            for($i = 0; $i < $this->w + 2; ++$i){
                $this->r_earr[$i] = 0;
                $this->g_earr[$i] = 0;
                $this->b_earr[$i] = 0;
            }
        }

        $this->r_nerr = 0;  /*Classification error for next pixel*/
        $this->g_nerr = 0;
        $this->b_nerr = 0;
    }

    function convert($cf, $alpha = 0) {
        $this->cf = $cf;
        $this->d_out = array();
        $this->alpha = $alpha;

        if($this->cf == self::CF_RAW || $this->cf == self::CF_RAW_ALPHA || $this->cf == self::CF_RAW_CHROMA) {
            $myfile = fopen($this->path, "r") or die("Unable to open file!");
            $this->d_out = unpack('C*', fread($myfile, filesize($this->path)));
            fclose($myfile);
            return;
        }

        $palette_size = 0;
        if($this->cf == self::CF_INDEXED_1_BIT) $palette_size = 2;
        if($this->cf == self::CF_INDEXED_2_BIT) $palette_size = 4;
        if($this->cf == self::CF_INDEXED_4_BIT) $palette_size = 16;
        if($this->cf == self::CF_INDEXED_8_BIT) $palette_size = 256;

        if($palette_size) {
            $img_tmp = imagecreatetruecolor($this->w, $this->h);
            imagecopy ($img_tmp, $this->img, 0 , 0 , 0 , 0, $this->w , $this->h);
            imagetruecolortopalette($this->img, false, $palette_size);
            $real_palette_size = imagecolorstotal($this->img);                          /*The real number of colos in the image's palette*/
	        for($i = 0; $i < $palette_size; $i++) {
	            if($i < $real_palette_size) {
	                $c = imagecolorsforindex ($this->img , $i);
                    array_push($this->d_out, $c['blue'], $c['green'], $c['red'], 0xFF);
                } else {
                    array_push($this->d_out, 0xFF, 0xFF, 0xFF, 0xFF);
                }
	        }
        }

        /*Convert all the pixels*/
        for($y = 0; $y < $this->h; $y++) {
            $this->dith_reset();

            for($x = 0; $x < $this->w; ++$x){
                $this->conv_px($x, $y);
            }
        }

        /*Revert the original image if it was converted to indexed*/
        if($palette_size) {
            imagecopy ($this->img, $img_tmp,  0 , 0 , 0 , 0 , $this->w , $this->h);
        }
    }

    function format_to_c_array() {

        $c_array = "";
        $i = 0;
        $y_end = $this->h;
        $x_end = $this->w;

        if($this->cf == self::CF_TRUE_COLOR_332) {
            $c_array .= "\n#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8";
            if(!$this->alpha) $c_array .= "\n  /*Pixel format: Red: 3 bit, Green: 3 bit, Blue: 2 bit*/";
            else  $c_array .= "\n  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/";
        } else if($this->cf == self::CF_TRUE_COLOR_565) {
            $c_array .= "\n#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0";
            if(!$this->alpha) $c_array .= "\n  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/";
            else $c_array .= "\n  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/";
        }  else if($this->cf == self::CF_TRUE_COLOR_565_SWAP) {
            $c_array .= "\n#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0";
            if(!$this->alpha) $c_array .=  "\n  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit BUT the 2 bytes are swapped*/";
            else $c_array .= "\n  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/";
        }  else if($this->cf == self::CF_TRUE_COLOR_888) {
            $c_array .= "\n#if LV_COLOR_DEPTH == 32";
            if(!$this->alpha) $c_array .= "\n  /*Pixel format: Fix 0xFF: 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/";
            else "\n  /*Pixel format: Alpha 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/";
        } else if($this->cf == self::CF_INDEXED_1_BIT) {
            $c_array .= "\n";
            for($p = 0; $p < 2; $p ++) {
                $c_array .= "  0x" . str_pad(dechex($this->d_out[$p * 4 + 0]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 1]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 2]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 3]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "\t/*Color of index $p*/\n";
            }

            $i = $p * 4;
        }
        else if($this->cf == self::CF_INDEXED_2_BIT) {
            $c_array .= "\n";
            for($p = 0; $p < 4; $p ++) {
                $c_array .= "  0x" . str_pad(dechex($this->d_out[$p * 4 + 0]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 1]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 2]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 3]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "\t/*Color of index $p*/\n";
            }

            $i = $p * 4;
        }
        else if($this->cf == self::CF_INDEXED_4_BIT) {
            $c_array .= "\n";
            for($p = 0; $p < 16; $p ++) {
                $c_array .= "  0x" . str_pad(dechex($this->d_out[$p * 4 + 0]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 1]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 2]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 3]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "\t/*Color of index $p*/\n";
            }

            $i = $p * 4;
        }
        else if($this->cf == self::CF_INDEXED_8_BIT) {
            $c_array .= "\n";
            for($p = 0; $p < 256; $p ++) {
                $c_array .= "  0x" . str_pad(dechex($this->d_out[$p * 4 + 0]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 1]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 2]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "0x" . str_pad(dechex($this->d_out[$p * 4 + 3]), 2, '0', STR_PAD_LEFT) . ", ";
                $c_array .= "\t/*Color of index $p*/\n";
            }

            $i = $p * 4;
        }
         else if($this->cf == self::CF_RAW || $this->cf == self::CF_RAW_ALPHA || $this->cf == self::CF_RAW_CHROMA) {
            $y_end = 1;
            $x_end = count($this->d_out);
            $i = 1;
         }


        for($y = 0; $y < $y_end; $y++) {
            $c_array .= "\n  ";
            for($x = 0; $x < $x_end; $x++) {
                if($this->cf == self::CF_TRUE_COLOR_332) {
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    if($this->alpha) {
                        $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                        $i++;
                    }
                }
                else if($this->cf == self::CF_TRUE_COLOR_565 || $this->cf == self::CF_TRUE_COLOR_565_SWAP) {
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    if($this->alpha) {
                        $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                        $i++;
                    }
                }
                else if($this->cf == self::CF_TRUE_COLOR_888) {

                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";  $i++;
                }
                else if($this->cf == self::CF_ALPHA_1_BIT || $this->cf == self::CF_INDEXED_1_BIT) {
                    if(($x & 0x7) == 0) {
                        $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                        $i++;
                    }
                }
                else if($this->cf == self::CF_ALPHA_2_BIT || $this->cf == self::CF_INDEXED_2_BIT) {
                    if(($x & 0x3) == 0) {
                        $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                        $i++;
                    }
                }
                else if($this->cf == self::CF_ALPHA_4_BIT || $this->cf == self::CF_INDEXED_4_BIT) {
                    if(($x & 0x1) == 0) {
                        $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                        $i++;
                    }
                }
                else if($this->cf == self::CF_ALPHA_8_BIT || $this->cf == self::CF_INDEXED_8_BIT) {
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                    $i++;
                }
                else if($this->cf == self::CF_RAW || $this->cf == self::CF_RAW_ALPHA || $this->cf == self::CF_RAW_CHROMA) {
                    $c_array .= "0x" . str_pad(dechex($this->d_out[$i]), 2, '0', STR_PAD_LEFT) . ", ";
                    if($i != 0 && (($i % 16) == 0)) $c_array .= "\n  ";
                    $i++;
                }
            }
        }

        if($this->cf == self::CF_TRUE_COLOR_332 || $this->cf == self::CF_TRUE_COLOR_565 || $this->cf == self::CF_TRUE_COLOR_565_SWAP || $this->cf == self::CF_TRUE_COLOR_888) {
            $c_array .= "\n#endif";
        }

        return $c_array;

    }

    function get_c_header() {
        $c_header =
        "#include \"lvgl/lvgl.h\"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

";
        $attr_name = "LV_ATTRIBUTE_IMG_" . strtoupper($this->out_name); 
        $c_header .= 
"#ifndef $attr_name
#define $attr_name
#endif

const LV_ATTRIBUTE_MEM_ALIGN $attr_name uint8_t " . $this->out_name . "_map[] = {";

        return $c_header;
    }

    function get_c_footer($cf) {
        $c_footer =
        "\n};\n
const lv_img_dsc_t " . $this->out_name . " = {
  .header.always_zero = 0,
  .header.w = " . $this->w . ",
  .header.h = " . $this->h . ",\n";

      if($cf == self::CF_TRUE_COLOR) $c_footer .= "  .data_size = " . $this->w * $this->h . " * LV_COLOR_SIZE / 8,\n  .header.cf = LV_IMG_CF_TRUE_COLOR,";
      else if($cf == self::CF_TRUE_COLOR_ALPHA) $c_footer .= "  .data_size = " . $this->w * $this->h . " * LV_IMG_PX_SIZE_ALPHA_BYTE,\n  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,";
      else if($cf == self::CF_TRUE_COLOR_CHROMA) $c_footer .= "  .data_size = " . $this->w * $this->h . " * LV_COLOR_SIZE / 8,\n  .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,";
      else if($cf == self::CF_ALPHA_1_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_ALPHA_1BIT,";
      else if($cf == self::CF_ALPHA_2_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_ALPHA_2BIT,";
      else if($cf == self::CF_ALPHA_4_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_ALPHA_4BIT,";
      else if($cf == self::CF_ALPHA_8_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_ALPHA_8BIT,";
      else if($cf == self::CF_INDEXED_1_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_INDEXED_1BIT,";
      else if($cf == self::CF_INDEXED_2_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_INDEXED_2BIT,";
      else if($cf == self::CF_INDEXED_4_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_INDEXED_4BIT,";
      else if($cf == self::CF_INDEXED_8_BIT) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_INDEXED_8BIT,";
      else if($cf == self::CF_RAW) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_RAW,";
      else if($cf == self::CF_RAW_ALPHA) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_RAW_ALPHA,";
      else if($cf == self::CF_RAW_CHROMA) $c_footer .= "  .data_size = " . count($this->d_out) . ",\n  .header.cf = LV_IMG_CF_RAW_CHROMA_KEYED,";

    $c_footer .= "\n  .data = " . $this->out_name . "_map,
};\n";

        return $c_footer;
    }

    function download_c($name, $cf = -1, $content = ""){
      global $offline;

      if(strlen($content) < 1) {
        $content = $this->format_to_c_array();
      }

      if($cf < 0) $cf = $this->cf;

        $out = $this->get_c_header() . $content . "" . $this->get_c_footer($cf);
        $name = $name . ".c";

      if($offline){
		$name = $this->output_dir.$name;
        $file = fopen($name, "w");
        fwrite($file, $out);
        fclose($file);
      }
      else{
        header('Content-Type: application/text');
        header('Content-disposition: attachment; filename='.$name);
        header('Content-Length: ' . strlen($out));

        echo($out);

      }
    }


    function download_bin($name, $cf = -1, $content = 0){
      global $offline;

      if($content == 0) {
        $content = $this->d_out;
      }

      if($cf < 0) $cf = $this->cf;
      $name .= ".bin";

      $lv_cf = 4;               /*Color format in LittlevGL*/
      switch($cf) {
        case self::CF_TRUE_COLOR:
            $lv_cf = 4; break;
        case self::CF_TRUE_COLOR_ALPHA:
            $lv_cf = 5; break;
        case self::CF_TRUE_COLOR_CHROMA:
            $lv_cf = 6; break;
        case self::CF_INDEXED_1_BIT:
            $lv_cf = 7; break;
        case self::CF_INDEXED_2_BIT:
            $lv_cf = 8; break;
        case self::CF_INDEXED_4_BIT:
            $lv_cf = 9; break;
        case self::CF_INDEXED_8_BIT:
            $lv_cf = 10; break;
        case self::CF_ALPHA_1_BIT:
            $lv_cf = 11; break;
        case self::CF_ALPHA_2_BIT:
            $lv_cf = 12; break;
        case self::CF_ALPHA_4_BIT:
            $lv_cf = 13; break;
        case self::CF_ALPHA_8_BIT:
            $lv_cf = 14; break;
      }


      $header = $lv_cf + ($this->w << 10) + ($this->h << 21);
      $header_bin = pack("V", $header);

      $content = pack("C*", ...$content);

      if($offline){
		$name = $this->output_dir.$name;
        $file = fopen($name, "w");
        fwrite($file, $header_bin);
        fwrite($file, $content);
        fclose($file);
      }
      else{
        $len = strlen($content) + 4;
        header('Content-Type: application/text');
        header('Content-disposition: attachment; filename='.$name);
        header('Content-Length:' . $len);
        echo($header_bin[0]);
        echo($header_bin[1]);
        echo($header_bin[2]);
        echo($header_bin[3]);
        echo($content);


      }
    }

    private function conv_px($x, $y) {
        $c = imagecolorat($this->img, $x, $y);
        if($this->alpha){
            $a = ($c & 0xff000000) >> 23;       /*Alpha is 7 bit*/
            if($a & 0x02) $a |= 0x01;           /*Repeate the last bit: 0000000 -> 00000000; 1111110 -> 11111111*/
            $a = 255 - $a;
        } else {
            $a = 0xff;
        }
        $r = ($c & 0x00ff0000) >> 16;
        $g = ($c & 0x0000ff00) >> 8;
        $b = ($c & 0x000000ff) >> 0;

        $this->dith_next($r, $g, $b, $x);

        if($this->cf == self::CF_TRUE_COLOR_332) {
           $c8 = ($this->r_act) | ($this->g_act >> 3) | ($this->b_act >> 6);	//RGB332
            array_push($this->d_out, $c8);
            if($this->alpha) array_push($this->d_out, $a);
        } else if($this->cf == self::CF_TRUE_COLOR_565) {
          $c16 = (($this->r_act) << 8) | (($this->g_act) << 3) | (($this->b_act) >> 3);	//RGR565
            array_push($this->d_out, $c16 & 0xFF);
            array_push($this->d_out, ($c16 >> 8) & 0xFF);
            if($this->alpha) array_push($this->d_out, $a);
        } else if($this->cf == self::CF_TRUE_COLOR_565_SWAP) {
            $c16 = (($this->r_act) << 8) | (($this->g_act) << 3) | (($this->b_act) >> 3);	//RGR565
            array_push($this->d_out, ($c16 >> 8) & 0xFF);
            array_push($this->d_out, $c16 & 0xFF);
            if($this->alpha) array_push($this->d_out, $a);
        } else if($this->cf == self::CF_TRUE_COLOR_888) {
            array_push($this->d_out, $this->b_act);
            array_push($this->d_out, $this->g_act);
            array_push($this->d_out, $this->r_act);
            array_push($this->d_out, $a);
        } else if($this->cf == self::CF_ALPHA_1_BIT) {
            $w = $this->w >> 3;
            if($this->w & 0x07) $w++;
            $p = $w * $y + ($x >> 3);
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            if($a > 0x80) {
                $this->d_out[$p] |= 1 << (7 - ($x & 0x7));
            }
        }
        else if($this->cf == self::CF_ALPHA_2_BIT) {
            $w = $this->w >> 2;
            if($this->w & 0x03) $w++;

            $p = $w * $y + ($x >> 2);
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            $this->d_out[$p] |= ($a >> 6) << (6 - (($x & 0x3) * 2));
        }
        else if($this->cf == self::CF_ALPHA_4_BIT) {
            $w = $this->w >> 1;
            if($this->w & 0x01) $w++;

            $p = $w * $y + ($x >> 1);
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            $this->d_out[$p] |= ($a >> 4) << (4 - (($x & 0x1) * 4));
        }
        else if($this->cf == self::CF_ALPHA_8_BIT) {
            $p = $this->w * $y + $x;
            $this->d_out[$p] = $a;
        }
        else if($this->cf == self::CF_INDEXED_1_BIT) {
            $w = $this->w >> 3;
            if($this->w & 0x07) $w++;

            $p = $w * $y + ($x >> 3) + 8;                       /* +8 for the palette*/
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            $this->d_out[$p] |= ($c & 0x1) << (7 - ($x & 0x7));
        }
        else if($this->cf == self::CF_INDEXED_2_BIT) {
            $w = $this->w >> 2;
            if($this->w & 0x03) $w++;

            $p = $w * $y + ($x >> 2) + 16;                              /* +16 for the palette*/
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            $this->d_out[$p] |= ($c & 0x3) << (6 - (($x & 0x3) * 2));
        }
        else if($this->cf == self::CF_INDEXED_4_BIT) {
            $w = $this->w >> 1;
            if($this->w & 0x01) $w++;

            $p = $w * $y + ($x >> 1) + 64;                              /* +64 for the palette*/
            if(!isset($this->d_out[$p])) $this->d_out[$p] = 0;          /*Clear the bits first*/
            $this->d_out[$p] |= ($c & 0xF) << (4 - (($x & 0x1) * 4));
        }
        else if($this->cf == self::CF_INDEXED_8_BIT) {
            $p = $this->w * $y + $x + 1024;                              /* +1024 for the palette*/
            $this->d_out[$p] = $c & 0xFF;
        }
	}

    private function dith_reset() {
        if($this->dith){
          $this->r_nerr = 0;
          $this->g_nerr = 0;
          $this->b_nerr = 0;
        }
    }

    private function dith_next($r, $g, $b, $x) {

     if($this->dith){
        $this->r_act = $r + $this->r_nerr + $this->r_earr[$x+1];
        $this->r_earr[$x+1] = 0;

        $this->g_act = $g + $this->g_nerr + $this->g_earr[$x+1];
        $this->g_earr[$x+1] = 0;

        $this->b_act = $b + $this->b_nerr + $this->b_earr[$x+1];
        $this->b_earr[$x+1] = 0;

        if($this->cf == self::CF_TRUE_COLOR_332) {
            $this->r_act = $this->classify_pixel($this->r_act, 3);
            $this->g_act = $this->classify_pixel($this->g_act, 3);
            $this->b_act = $this->classify_pixel($this->b_act, 2);

            if($this->r_act > 0xE0) $this->r_act = 0xE0;
            if($this->g_act > 0xE0) $this->g_act = 0xE0;
            if($this->b_act > 0xC0) $this->b_act = 0xC0;

        } else if($this->cf == self::CF_TRUE_COLOR_565 || $this->cf == self::CF_TRUE_COLOR_565_SWAP) {
            $this->r_act = $this->classify_pixel($this->r_act, 5);
            $this->g_act = $this->classify_pixel($this->g_act, 6);
            $this->b_act = $this->classify_pixel($this->b_act, 5);

            if($this->r_act > 0xF8) $this->r_act = 0xF8;
            if($this->g_act > 0xFC) $this->g_act = 0xFC;
            if($this->b_act > 0xF8) $this->b_act = 0xF8;

        } else if($this->cf == self::CF_TRUE_COLOR_888) {
            $this->r_act = $this->classify_pixel($this->r_act, 8);
            $this->g_act = $this->classify_pixel($this->g_act, 8);
            $this->b_act = $this->classify_pixel($this->b_act, 8);

            if($this->r_act > 0xFF) $this->r_act = 0xFF;
            if($this->g_act > 0xFF) $this->g_act = 0xFF;
            if($this->b_act > 0xFF) $this->b_act = 0xFF;
        }

        $this->r_err = $r - $this->r_act;
        $this->g_err = $g - $this->g_act;
        $this->b_err = $b - $this->b_act;

        $this->r_nerr = round((7 * $this->r_err) / 16);
        $this->g_nerr = round((7 * $this->g_err) / 16);
        $this->b_nerr = round((7 * $this->b_err) / 16);

        $this->r_earr[$x] += round((3 * $this->r_err) / 16);
        $this->g_earr[$x] += round((3 * $this->g_err) / 16);
        $this->b_earr[$x] += round((3 * $this->b_err) / 16);

        $this->r_earr[$x+1] += round((5 * $this->r_err) / 16);
        $this->g_earr[$x+1] += round((5 * $this->g_err) / 16);
        $this->b_earr[$x+1] += round((5 * $this->b_err) / 16);

        $this->r_earr[$x+2] += round($this->r_err / 16);
        $this->g_earr[$x+2] += round($this->g_err / 16);
        $this->b_earr[$x+2] += round($this->b_err / 16);
      }
      else{
        if($this->cf == self::CF_TRUE_COLOR_332) {
            $this->r_act = $this->classify_pixel($r, 3);
            $this->g_act = $this->classify_pixel($g, 3);
            $this->b_act = $this->classify_pixel($b, 2);

            if($this->r_act > 0xE0) $this->r_act = 0xE0;
            if($this->g_act > 0xE0) $this->g_act = 0xE0;
            if($this->b_act > 0xC0) $this->b_act = 0xC0;

        } else if($this->cf == self::CF_TRUE_COLOR_565 || $this->cf == self::CF_TRUE_COLOR_565_SWAP) {
            $this->r_act = $this->classify_pixel($r, 5);
            $this->g_act = $this->classify_pixel($g, 6);
            $this->b_act = $this->classify_pixel($b, 5);

            if($this->r_act > 0xF8) $this->r_act = 0xF8;
            if($this->g_act > 0xFC) $this->g_act = 0xFC;
            if($this->b_act > 0xF8) $this->b_act = 0xF8;

        } else if($this->cf == self::CF_TRUE_COLOR_888) {
            $this->r_act = $this->classify_pixel($r, 8);
            $this->g_act = $this->classify_pixel($g, 8);
            $this->b_act = $this->classify_pixel($b, 8);

            if($this->r_act > 0xFF) $this->r_act = 0xFF;
            if($this->g_act > 0xFF) $this->g_act = 0xFF;
            if($this->b_act > 0xFF) $this->b_act = 0xFF;
        }
      }
    }

    private function classify_pixel($value, $bits){
      $tmp = 1 << (8 - $bits);
      $val = round($value / $tmp, 0, PHP_ROUND_HALF_DOWN) * $tmp;
      if($val < 0) $val = 0;
      return $val;
    }
}

$offline = 0;
if (!isset($_SERVER["HTTP_HOST"])) {
  parse_str($argv[1], $_POST);
  $offline = 1;
}


if($offline == 0){
  /*The scripts runs ONLINE (likely on littelvgl.com)*/
  header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
  header("Cache-Control: post-check=0, pre-check=0", false);
  header("Pragma: no-cache");

  $img_file = $_FILES["img_file"]["tmp_name"];
  $img_file_name = $_FILES["img_file"]["name"];
  $output_name = $_POST["name"];
  $cf = $_POST["cf"];
  $format = $_POST["format"];
  $dith = $_POST["dith"];
}
else{
  /*The scripts runs OFFLINE (likely in command)*/
  if(isset($_POST["name"])){
    $output_name = $_POST["name"];
    echo"Convert:".$output_name;
  }
  else{
    echo("Mising Name\n");
    exit(0);
  }

  if(isset($_POST["img"])){
    $img_file = $_POST["img"];
    $img_file_name = $_POST["img"];
	echo "\nimg=".$img_file;
  }
  else{
    echo("Mising image file\n");
    exit(0);
  }

  if(isset($_POST["format"])){
    $format = $_POST["format"];
  }
  else{
    $format = "c_array";
  }

  if(isset($_POST["dith"])){
    $dith = $_POST["dith"];
  }
  else {
    $dith = 0;
  }
  if(isset($_POST["outdir"]))
  {
	$outdir = $_POST["outdir"];
  }else
  {
	  $outdir = "./";
	  echo "\noutdir=".$outdir;
  }
  if(isset($_POST["cf"])){
    $cf = $_POST["cf"];
  }
  else {
    $cf = "true_color";
  }
}

$conv = new Converter($img_file, $img_file_name,$outdir, $output_name, $dith);

if(!strcmp($format, "c_array")) {
    if(!strcmp($cf, "true_color") || !strcmp($cf, "true_color_alpha") || !strcmp($cf, "true_color_chroma")) {
        $alpha = 0;
        if(!strcmp($cf, "true_color_alpha"))  $alpha = 1;

        $conv->convert($conv::CF_TRUE_COLOR_332, $alpha);
        $c_332 = $conv->format_to_c_array();

        $conv->convert($conv::CF_TRUE_COLOR_565, $alpha);
        $c_565 = $conv->format_to_c_array();

        $conv->convert($conv::CF_TRUE_COLOR_565_SWAP, $alpha);
        $c_565_swap = $conv->format_to_c_array();

        $conv->convert($conv::CF_TRUE_COLOR_888, $alpha);
        $c_888 = $conv->format_to_c_array();

        $c_res = $c_332 . $c_565 . $c_565_swap . $c_888;

        if(!strcmp($cf, "true_color")) $conv->download_c($conv->out_name, $conv::CF_TRUE_COLOR, $c_res);
        if(!strcmp($cf, "true_color_alpha")) $conv->download_c($conv->out_name, $conv::CF_TRUE_COLOR_ALPHA, $c_res);
        if(!strcmp($cf, "true_color_chroma")) $conv->download_c($conv->out_name, $conv::CF_TRUE_COLOR_CHROMA, $c_res);
    }
    else if(!strcmp($cf, "alpha_1")) {
         $conv->convert($conv::CF_ALPHA_1_BIT, 1);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_2")) {
         $conv->convert($conv::CF_ALPHA_2_BIT, 1);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_4")) {
         $conv->convert($conv::CF_ALPHA_4_BIT, 1);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_8")) {
         $conv->convert($conv::CF_ALPHA_8_BIT, 1);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_1")) {
         $conv->convert($conv::CF_INDEXED_1_BIT);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_2")) {
         $conv->convert($conv::CF_INDEXED_2_BIT);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_4")) {
         $conv->convert($conv::CF_INDEXED_4_BIT);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_8")) {
         $conv->convert($conv::CF_INDEXED_8_BIT);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "raw")) {
         $conv->convert($conv::CF_RAW);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "raw_alpha")) {
         $conv->convert($conv::CF_RAW_ALPHA, 1);
         $conv->download_c($conv->out_name);
    }
    else if(!strcmp($cf, "raw_chroma")) {
         $conv->convert($conv::CF_RAW_CHROMA);
         $conv->download_c($conv->out_name);
    }
}
/*Binary download*/
else  {
    if(!strcmp($cf, "true_color") || !strcmp($cf, "true_color_alpha") || !strcmp($cf, "true_color_chroma")) {
        $alpha = 0;
        if(!strcmp($cf, "true_color_alpha")) $alpha = 1;

        if (!strcmp($format, "bin_332"))            $conv->convert($conv::CF_TRUE_COLOR_332, $alpha);
        else if (!strcmp($format, "bin_565"))       $conv->convert($conv::CF_TRUE_COLOR_565, $alpha);
        else if (!strcmp($format, "bin_565_swap"))  $conv->convert($conv::CF_TRUE_COLOR_565_SWAP, $alpha);
        else if (!strcmp($format, "bin_888"))       $conv->convert($conv::CF_TRUE_COLOR_888, $alpha);
        else {
         echo("Unknown output file format: $format");
         exit(1);
        }
        if(!strcmp($cf, "true_color"))          $conv->download_bin($conv->out_name, $conv::CF_TRUE_COLOR);
        if(!strcmp($cf, "true_color_alpha"))    $conv->download_bin($conv->out_name, $conv::CF_TRUE_COLOR_ALPHA);
        if(!strcmp($cf, "true_color_chroma"))   $conv->download_bin($conv->out_name, $conv::CF_TRUE_COLOR_CHROMA);
    }
    else if(!strcmp($cf, "alpha_1")) {
         $conv->convert($conv::CF_ALPHA_1_BIT, 1);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_2")) {
         $conv->convert($conv::CF_ALPHA_2_BIT, 1);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_4")) {
         $conv->convert($conv::CF_ALPHA_4_BIT, 1);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "alpha_8")) {
         $conv->convert($conv::CF_ALPHA_8_BIT, 1);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_1")) {
         $conv->convert($conv::CF_INDEXED_1_BIT);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_2")) {
         $conv->convert($conv::CF_INDEXED_2_BIT);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_4")) {
         $conv->convert($conv::CF_INDEXED_4_BIT);
         $conv->download_bin($conv->out_name);
    }
    else if(!strcmp($cf, "indexed_8")) {
         $conv->convert($conv::CF_INDEXED_8_BIT);
         $conv->download_bin($conv->out_name);
    }
}




/*
$c_array .= "#include <stdint.h>
            #include \"lv_conf.h\"
            #include \"lvgl/lv_draw/lv_draw.h\""; */


//download("test", $c_565);

?>

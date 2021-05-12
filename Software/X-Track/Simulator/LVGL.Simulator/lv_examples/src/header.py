#!/opt/bin/lv_micropython -i
try:
  import sys
except ImportError:
  import usys as sys
# JS requires a special import 
if sys.platform == 'javascript':
  import utime as time
  import imp
  sys.path.append('https://raw.githubusercontent.com/littlevgl/lv_binding_micropython/master/lib')
  import display_driver
else:
  import display_driver
  import time

import lvgl as lv

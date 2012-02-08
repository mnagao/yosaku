#!/usr/bin/python

import grabkey

def hotkey_callback(arg):
    print "in hotkey_callback: arg = ", arg

grabkey.set_callback(hotkey_callback)
grabkey.set_grabkey(34)
grabkey.start_grabkey()

print "yosaku exit."


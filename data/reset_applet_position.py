#!/usr/bin/python3

import subprocess
import signal
import time
import sys

from gi.repository import Gio
from gi.repository import Gdk
from gi.repository import GLib


class PanelMonitor:

    def __init__(self):

        self.loop = GLib.MainLoop.new(None, False)
        signal.signal(signal.SIGINT, self.quit)

        screen = Gdk.Screen.get_default()
        screen.connect('monitors-changed', self.restart_panel)

        self.seconds = 30
        GLib.timeout_add(1000, self._check_panel_running)

    def _check_panel_running(self):

        self.seconds -= 1

        if self.seconds > 0:
            panel_is_running = False
            try:
                ret = subprocess.call('ps -aux | grep ukui-menu.py | grep python',
                                      stdout = subprocess.DEVNULL, shell = True)
                if ret == 0:
                    panel_is_running = True
            except Exception:
                pass

            #if panel_is_running and self._check_need_replace():
            if panel_is_running == False:
                #self.restart_panel(None)
                try:
                    subprocess.call('ukui-menu &',
                                    stdout = subprocess.DEVNULL,
                                    stderr = subprocess.DEVNULL,
                                    shell = True)
                except Exception:
                    pass
                return False
            else:
                return True
        else:
            return False

    def _tray_settings(self):
        panel_settings = Gio.Settings.new('org.mate.panel')
        panel_objects = panel_settings.get_strv('object-id-list')

        tray_settings = None
        for obj_name in panel_objects:
            obj_settings = Gio.Settings.new_with_path('org.mate.panel.object',
                                                      '/org/mate/panel/objects/%s/' % obj_name)
            applet_iid = obj_settings.get_string('applet-iid')
    
            if applet_iid == 'MateIndicatorsAppletFactory::MateIndicatorsApplet':
                tray_settings = obj_settings
                break

        return tray_settings

    def restart_panel(self, screen):

        self.set_tray_position()
        time.sleep(1)
        try:
            subprocess.call('killall mate-panel',
                            stdout = subprocess.DEVNULL,
                            stderr = subprocess.DEVNULL,
                            shell = True)
        except Exception:
            pass

    def set_tray_position(self):
        tray_settings = self._tray_settings()
        
        if not tray_settings:
            return

        tray_settings.set_boolean('panel-right-stick', True)
        tray_settings.set_int('position', 0)

    def _check_need_replace(self):

        tray_settings = self._tray_settings()

        if not tray_settings:
            return False

        need_reboot = False
        if not tray_settings.get_boolean('panel-right-stick') or tray_settings.get_int('position') != 0:
            need_reboot = True

        return need_reboot

    def run(self):
        self.loop.run()

    def quit(self, signum, sigh):
        self.loop.quit()


if __name__ == '__main__':

    app = Gio.Application.new('reset_applet_position.py', Gio.ApplicationFlags.IS_SERVICE)
    try:
        if not app.register(None):
            sys.exit(1)
    except GLib.Error:
        sys.exit(1)

    pm = PanelMonitor()
    pm.run()


from platformio.managers.platform import PlatformBase
import os
class Fdv32Platform(PlatformBase):

    def get_boards(self, id_=None):
        result = PlatformBase.get_boards(self, id_)
        if not result:
            return result
        if id_:
            return self._add_dynamic_options(result)
        else:
            for key, value in result.items():
                result[key] = self._add_dynamic_options(result[key])
            return result

    def _add_dynamic_options(self, board):

        # debug tools
        debug = board.manifest.get("debug", {})
        if "tools" not in debug:
            debug['tools'] = {}

        debug_script = os.path.join(os.path.realpath('.'),'openocd.cfg')
        if not os.path.exists(debug_script):
            debug_script = os.path.join(os.path.dirname(__file__), "misc", board.id + ".openocd.cfg")
        # upload_script = upload_script.replace('\\','/')
        server_args = [
            "-f", debug_script
        ]

        debug['tools']['sipeed-rv-debugger'] = {
            "server": {
                "package": "tool-openocd-slink",
                "executable": "bin/openocd",
                "arguments": server_args
            }
        }

        debug['debug_port'] = 'localhost:3333'

        board.manifest['debug'] = debug
        return board        
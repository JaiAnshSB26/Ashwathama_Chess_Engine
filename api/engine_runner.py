# import subprocess
# import threading
# import queue
# import time
# import os
# import sys

# # --- CONFIG ---
# # Adjust this if your engine binary name is different or on Linux/Mac
# ENGINE_PATH = os.path.join(
#     os.path.dirname(__file__),
#     "../engine.exe"
# )

# if not os.path.exists(ENGINE_PATH):
#     print(f"[FATAL] Engine not found at {ENGINE_PATH}", file=sys.stderr)

# class EngineRunner:
#     def __init__(self):
#         # start engine process
#         self.proc = subprocess.Popen(
#             ENGINE_PATH,
#             stdin=subprocess.PIPE,
#             stdout=subprocess.PIPE,
#             stderr=subprocess.STDOUT,
#             text=True,           # get str not bytes
#             bufsize=1,           # line-buffered
#         )

#         # async reader thread -> pushes engine stdout lines into a Queue
#         self.q = queue.Queue()
#         self.reader_thread = threading.Thread(
#             target=self._read_output,
#             daemon=True
#         )
#         self.reader_thread.start()

#         # UCI init handshake
#         self._uci_init()

#     def _read_output(self):
#         """runs in thread: read engine stdout continuously"""
#         for line in self.proc.stdout:
#             line = line.strip()
#             if line:
#                 # for debugging:
#                 # print("[ENGINE]", line)
#                 self.q.put(line)

#     def send(self, cmd: str):
#         """send a command to engine stdin"""
#         # for debugging:
#         # print("[SEND]", cmd)
#         self.proc.stdin.write(cmd + "\n")
#         self.proc.stdin.flush()

#     def _wait_for_token(self, token: str, timeout: float = 5.0):
#         """wait until engine outputs a line containing token"""
#         deadline = time.time() + timeout
#         while time.time() < deadline:
#             try:
#                 line = self.q.get(timeout=0.1)
#                 if token in line:
#                     return True
#             except queue.Empty:
#                 pass
#         return False

#     def _uci_init(self):
#         """standard UCI startup handshake"""
#         self.send("uci")
#         ok = self._wait_for_token("uciok", timeout=5.0)
#         if not ok:
#             print("[WARN] didn't see uciok")

#         self.send("isready")
#         ready = self._wait_for_token("readyok", timeout=5.0)
#         if not ready:
#             print("[WARN] didn't see readyok")

#     def get_bestmove(self, fen: str, movetime_ms: int = 500):
#         """
#         Ask engine for best move and eval from a FEN.
#         Returns (bestmove: 'e2e4', evalScoreFloat: 0.34)

#         evalScoreFloat is in pawns from White's perspective.
#         """

#         # Special case:
#         # Some engines expect either:
#         #   position startpos
#         # or:
#         #   position fen <fen-string>
#         #
#         # We'll support both:
#         if fen == "startpos":
#             self.send("position startpos")
#         else:
#             self.send(f"position fen {fen}")

#         # ask it to think for a fixed amount of time
#         self.send(f"go movetime {movetime_ms}")

#         best_move = None
#         eval_score = 0.0  # default neutral

#         deadline = time.time() + (movetime_ms / 1000.0) + 2.0
#         while time.time() < deadline:
#             try:
#                 line = self.q.get(timeout=0.1)

#                 # Typical UCI info line looks like:
#                 # info depth 8 score cp 32 ...
#                 # or in mating lines: score mate 3
#                 if "score cp" in line:
#                     # parse centipawn
#                     parts = line.split()
#                     # find "cp", read next token
#                     if "cp" in parts:
#                         idx = parts.index("cp")
#                         if idx + 1 < len(parts):
#                             try:
#                                 cp_val = int(parts[idx + 1])
#                                 eval_score = cp_val / 100.0
#                             except:
#                                 pass
#                 elif "score mate" in line:
#                     # Mate in N means basically +/- huge
#                     # We'll approximate: mate in X = ±100
#                     parts = line.split()
#                     if "mate" in parts:
#                         idx = parts.index("mate")
#                         if idx + 1 < len(parts):
#                             try:
#                                 mate_val = int(parts[idx + 1])
#                                 # positive mate_val means winning (white mates)
#                                 # negative means losing
#                                 # We'll just squash into a big eval
#                                 if mate_val > 0:
#                                     eval_score = 100.0
#                                 else:
#                                     eval_score = -100.0
#                             except:
#                                 pass

#                 # bestmove e2e4
#                 if line.startswith("bestmove"):
#                     tokens = line.split()
#                     if len(tokens) >= 2:
#                         best_move = tokens[1]
#                     break

#             except queue.Empty:
#                 continue

#         return best_move, eval_score


# # Create a singleton so the engine boots once, not per request
# engine = EngineRunner()

import subprocess
import threading
import queue
import time
import os
import sys

# --- CONFIG ---
ENGINE_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "engine.exe"))

if not os.path.exists(ENGINE_PATH):
    print(f"[FATAL] Engine not found at {ENGINE_PATH}", file=sys.stderr)


class EngineRunner:
    def __init__(self):
        # Start engine process (use list, not string, for Windows safety)
        self.proc = subprocess.Popen(
            [ENGINE_PATH, "--uci"],  # <-- run in UCI mode directly
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,  # line-buffered
            universal_newlines=True,
        )

        # Queue for async stdout reading
        self.q = queue.Queue()
        self.reader_thread = threading.Thread(target=self._read_output, daemon=True)
        self.reader_thread.start()

        # Wait a moment for engine to boot fully before handshake
        time.sleep(0.3)
        self._uci_init()

    def _read_output(self):
        """Continuously read lines from engine stdout"""
        for line in self.proc.stdout:
            if line:
                line = line.strip()
                # print("[ENGINE]", line)
                self.q.put(line)

    def send(self, cmd: str):
        """Send command to engine stdin"""
        if self.proc.poll() is not None:
            raise RuntimeError("Engine process has exited")
        try:
            self.proc.stdin.write(cmd + "\n")
            self.proc.stdin.flush()
        except Exception as e:
            print(f"[ERROR] Failed to send command '{cmd}': {e}", file=sys.stderr)

    def _wait_for_token(self, token: str, timeout: float = 5.0):
        """Wait until engine outputs a line containing token"""
        deadline = time.time() + timeout
        while time.time() < deadline:
            try:
                line = self.q.get(timeout=0.1)
                if token in line:
                    return True
            except queue.Empty:
                continue
        return False

    def _uci_init(self):
        """Perform standard UCI handshake"""
        self.send("uci")
        if not self._wait_for_token("uciok", 5):
            print("[WARN] Did not receive 'uciok'")
        self.send("isready")
        if not self._wait_for_token("readyok", 5):
            print("[WARN] Did not receive 'readyok'")

    def get_bestmove(self, fen: str, movetime_ms: int = 500):
        """Ask engine for best move and eval; return (bestmove, eval_score)"""
        if not fen:
            return None, 0.0

        if fen == "startpos":
            self.send("position startpos")
        else:
            self.send(f"position fen {fen}")

        self.send(f"go movetime {movetime_ms}")

        best_move = None
        eval_score = 0.0
        deadline = time.time() + (movetime_ms / 1000.0) + 2.0

        while time.time() < deadline:
            try:
                line = self.q.get(timeout=0.1)
                if "score cp" in line:
                    parts = line.split()
                    if "cp" in parts:
                        idx = parts.index("cp")
                        if idx + 1 < len(parts):
                            try:
                                eval_score = int(parts[idx + 1]) / 100.0
                            except:
                                pass
                elif "score mate" in line:
                    if "mate" in line:
                        mate_val = int(line.split("mate")[1].split()[0])
                        eval_score = 100.0 if mate_val > 0 else -100.0

                if line.startswith("bestmove"):
                    tokens = line.split()
                    if len(tokens) >= 2:
                        best_move = tokens[1]
                    break
            except queue.Empty:
                continue

        return best_move, eval_score


# --- Create singleton ---
engine = EngineRunner()

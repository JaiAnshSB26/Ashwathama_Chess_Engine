import subprocess
import threading
import queue
import time
import os
import sys

# pick correct engine binary depending on OS
if os.name == "nt":  # Windows
    ENGINE_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "engine.exe"))
else:  # Linux/GCP
    ENGINE_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "engine"))

if not os.path.exists(ENGINE_PATH):
    print(f"[FATAL] Engine not found at {ENGINE_PATH}", file=sys.stderr)

class EngineRunner:
    def __init__(self):
        # launch engine in UCI mode
        self.proc = subprocess.Popen(
            [ENGINE_PATH, "--uci"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
            universal_newlines=True,
        )

        self.q = queue.Queue()
        self.reader_thread = threading.Thread(target=self._read_output, daemon=True)
        self.reader_thread.start()

        time.sleep(0.3)  # give engine time to boot
        self._uci_init()

    def _read_output(self):
        for line in self.proc.stdout:
            if line:
                line = line.strip()
                print("[ENGINE RAW]", line)
                self.q.put(line)

    def send(self, cmd: str):
        if self.proc.poll() is not None:
            raise RuntimeError("Engine process has exited")
        try:
            self.proc.stdin.write(cmd + "\n")
            self.proc.stdin.flush()
        except Exception as e:
            print(f"[ERROR] Failed to send command '{cmd}': {e}", file=sys.stderr)

    def _wait_for_token(self, token: str, timeout: float = 5.0):
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
        self.send("uci")
        if not self._wait_for_token("uciok", 5):
            print("[WARN] Did not receive 'uciok'")
        self.send("isready")
        if not self._wait_for_token("readyok", 5):
            print("[WARN] Did not receive 'readyok'")

    def _read_bestmove_block(self, movetime_ms: int):
        best_move = None
        eval_cp = None # default if none received 0.0 before.

        # be generous: movetime + 5 seconds safety
        deadline = time.time() + (movetime_ms / 1000.0) + 15.0  #Good safety buffer for deep searches.

        while time.time() < deadline:
            try:
                line = self.q.get(timeout=0.1)
            except queue.Empty:
                continue

            # debug visibility
            # (optional, but can help)
            # print("[ENGINE READ]", line)

            # eval parsing
            if "score cp" in line:
                parts = line.split()
                try:
                    i = parts.index("cp")
                    eval_cp = int(parts[i+1])
                except Exception:
                    pass

            elif "score mate" in line:
                parts = line.split()
                try:
                    i = parts.index("mate")
                    mate = int(parts[i+1])
                    eval_cp = 100000 if mate > 0 else -100000
                except Exception:
                    pass

            # the golden signal:
            if line.startswith("bestmove"):
                tokens = line.split()
                if len(tokens) >= 2:
                    best_move = tokens[1]
                break
        # default to 0 if engine never reported a score (keeps API consistent)
        if eval_cp is None:
            eval_cp = 0

        # return cp, not already divided
        return best_move, eval_cp


    def get_bestmove_from_moves(self, moves_list, movetime_ms=500):
        """
        moves_list: ["e2e4", "e7e5", "g1f3", ...]
        We'll reconstruct the full game every call so engine never desyncs.
        """
        if not moves_list:
            self.send("position startpos")
        else:
            joined = " ".join(moves_list)
            self.send(f"position startpos moves {joined}")

        self.send(f"go movetime {movetime_ms}")

        return self._read_bestmove_block(movetime_ms)

# singleton engine
engine = EngineRunner()

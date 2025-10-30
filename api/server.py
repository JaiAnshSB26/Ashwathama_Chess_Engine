from flask import Flask, request, jsonify
from flask_cors import CORS
from engine_runner import engine

app = Flask(__name__)
# CORS(app)
# CORS(app, resources={r"/*": {
#     "origins": ["http://localhost:5173", "http://127.0.0.1:5173"],
#     "supports_credentials": False,
#     "allow_headers": ["Content-Type"],
#     "methods": ["GET", "POST", "OPTIONS"]
# }})
CORS(app, resources={r"/*": {"origins": "*"}})

@app.route("/health", methods=["GET"])
def health():
    return jsonify({"ok": True})

@app.route("/move", methods=["POST"])
def move():
    if request.method == "OPTIONS":
        return ("", 204)
    data = request.get_json(force=True)

    moves_list = data.get("moves", [])
    movetime = data.get("movetime", 500)

    print(f"[/move] moves_list = {moves_list} movetime = {movetime}")

    bestmove, eval_score = engine.get_bestmove_from_moves(
        moves_list,
        movetime_ms=movetime,
    )

    print(f"[/move] engine returned: {bestmove} {eval_score}")

    # HARDENING: don't 500 if engine fails to emit a move
    if not bestmove:
        print("!!! ENGINE FAILED TO MOVE on sequence:", moves_list)

        return jsonify({
            "bestmove": None,
            "eval": eval_score,
            "note": "engine had no move (likely internal fail)",
            "moves_seen": moves_list,
        }), 200

    return jsonify({
        "bestmove": bestmove,
        "eval": eval_score,
    }), 200

if __name__ == "__main__":
    # local dev
    print(">>> Starting Flask on 127.0.0.1:5055")
    app.run(host="127.0.0.1", port=5055, debug=True)
    # app.run(host="0.0.0.0", port=5055, debug=True)
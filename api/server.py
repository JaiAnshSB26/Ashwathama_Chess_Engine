from flask import Flask, request, jsonify
from flask_cors import CORS
from engine_runner import engine

app = Flask(__name__)
CORS(app)  # allow frontend http://localhost:5173 etc to call us

@app.route("/health", methods=["GET"])
def health():
    return jsonify({"ok": True})

@app.route("/move", methods=["POST"])
def move():
    data = request.get_json(force=True)

    fen = data.get("fen")
    movetime = data.get("movetime", 500)  # ms, frontend can tweak later

    if not fen:
        return jsonify({"error": "No FEN provided"}), 400

    bestmove, eval_score = engine.get_bestmove(fen, movetime_ms=movetime)

    if not bestmove:
        return jsonify({
            "error": "Engine did not return a move",
            "eval": eval_score
        }), 500

    return jsonify({
        "bestmove": bestmove,
        "eval": eval_score
    }), 200

if __name__ == "__main__":
    # local dev mode (not production)
    app.run(host="0.0.0.0", port=5001, debug=True)

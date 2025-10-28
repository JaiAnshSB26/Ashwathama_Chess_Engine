"""
Flask API for Ashwathama Chess Engine

Wraps the UCI chess engine with HTTP endpoints for web frontend.
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import os
import sys

app = Flask(__name__)
CORS(app)  # Allow requests from frontend (Vercel)

# Path to compiled engine binary
if os.name == 'nt':  # Windows
    ENGINE_PATH = os.path.join(os.path.dirname(__file__), '..', 'engine.exe')
else:  # Linux/Mac (GCP)
    ENGINE_PATH = os.path.join(os.path.dirname(__file__), '..', 'engine')


def get_engine_move(fen, depth=5):
    """
    Get best move from engine for given position.
    
    Args:
        fen: Board position in FEN notation
        depth: Search depth (default 5)
    
    Returns:
        UCI move string (e.g., "e2e4") or None if error
    """
    try:
        # Start engine in UCI mode
        process = subprocess.Popen(
            [ENGINE_PATH, '--uci'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # Send UCI commands
        commands = f"uci\nisready\nposition fen {fen}\ngo depth {depth}\nquit\n"
        stdout, stderr = process.communicate(commands, timeout=30)
        
        # Parse output for bestmove
        for line in stdout.splitlines():
            if line.startswith('bestmove'):
                move = line.split()[1]
                return move
        
        return None
    
    except subprocess.TimeoutExpired:
        process.kill()
        return None
    except Exception as e:
        print(f"Engine error: {e}", file=sys.stderr)
        return None


@app.route('/')
def home():
    """Health check endpoint"""
    return jsonify({
        'engine': 'Ashwathama',
        'version': '1.0',
        'status': 'online',
        'endpoints': ['/move', '/info']
    })


@app.route('/info')
def info():
    """Get engine information"""
    return jsonify({
        'name': 'Ashwathama',
        'version': '1.0',
        'authors': [
            'Jai Ansh Bindra',
            'Kingshuk Gupta',
            'Mohamed Iyad Boualem',
            'Ziji Wang',
            'Tomas Gradowski'
        ],
        'protocol': 'UCI',
        'features': ['FEN parsing', 'Alpha-beta search', 'Position evaluation']
    })


@app.route('/move', methods=['POST'])
def get_move():
    """
    Calculate best move for given position.
    
    Request body:
        {
            "fen": "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "depth": 5  (optional, default 5)
        }
    
    Response:
        {
            "bestmove": "e2e4",
            "fen": "...",
            "success": true
        }
    """
    data = request.get_json()
    
    if not data or 'fen' not in data:
        return jsonify({'error': 'FEN position required'}), 400
    
    fen = data['fen']
    depth = data.get('depth', 5)
    
    # Validate depth
    if not isinstance(depth, int) or depth < 1 or depth > 10:
        return jsonify({'error': 'Depth must be between 1 and 10'}), 400
    
    # Get move from engine
    bestmove = get_engine_move(fen, depth)
    
    if bestmove:
        return jsonify({
            'success': True,
            'bestmove': bestmove,
            'fen': fen,
            'depth': depth
        })
    else:
        return jsonify({
            'success': False,
            'error': 'Engine failed to calculate move'
        }), 500


@app.route('/health')
def health():
    """Kubernetes/GCP health check"""
    return jsonify({'status': 'healthy'}), 200


if __name__ == '__main__':
    # Check if engine exists
    if not os.path.exists(ENGINE_PATH):
        print(f"ERROR: Engine not found at {ENGINE_PATH}")
        print("Please run build.bat (Windows) or build.sh (Linux) first")
        sys.exit(1)
    
    # Run server
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=True)

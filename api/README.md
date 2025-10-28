# Ashwathama Chess Engine API

Flask backend that wraps the UCI chess engine.

## Setup

```bash
# Install dependencies
pip install -r requirements.txt

# Make sure engine is compiled
cd ..
./build.bat  # Windows
# or
./build.sh   # Linux/Mac

# Run API server
python server.py
```

## API Endpoints

### `GET /`
Health check and API info

### `GET /info`
Engine information and authors

### `POST /move`
Get best move for a position

**Request:**
```json
{
    "fen": "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "depth": 5
}
```

**Response:**
```json
{
    "success": true,
    "bestmove": "e2e4",
    "fen": "...",
    "depth": 5
}
```

## Testing

```bash
curl http://localhost:5000/info

curl -X POST http://localhost:5000/move \
  -H "Content-Type: application/json" \
  -d '{"fen":"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"}'
```

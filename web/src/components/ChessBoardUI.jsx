import { useState, useEffect } from "react";
import { Chess } from "chess.js";
import MoveHistory from "./MoveHistory.jsx";

const PIECE_UNICODE = {
  p: "♟",
  r: "♜",
  n: "♞",
  b: "♝",
  q: "♛",
  k: "♚",
  P: "♙",
  R: "♖",
  N: "♘",
  B: "♗",
  Q: "♕",
  K: "♔",
};

export default function ChessBoardUI() {
  const [game, setGame] = useState(() => new Chess());
  const [boardState, setBoardState] = useState(() => game.board());

  // board layout / selection
  const [selectedSquare, setSelectedSquare] = useState(null);
  const [legalTargets, setLegalTargets] = useState([]);

  // responsive sizing
  const [boardPx, setBoardPx] = useState(420);
  useEffect(() => {
    function handleResize() {
      const vh = window.innerHeight;
      const vw = window.innerWidth;

      // space for header etc.
      const maxByHeight = vh - 220;
      const maxByWidth = vw * 0.6; // leave room for sidebar on desktop

      //   const size = Math.min(maxByHeight, maxByWidth, 480);
      const size = Math.min(maxByHeight * 0.9, maxByWidth, 520);
      setBoardPx(size < 260 ? 260 : size);
    }
    handleResize();
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  // move history (SAN notation)
  const [moveHistory, setMoveHistory] = useState([]); // e.g. ["e4", "e5", "Nf3"...]

  // placeholder eval info (will come from engine later)
  const [evalScore, setEvalScore] = useState(0.0); // +0.3 means white better, -1.2 black better
  const [thinking, setThinking] = useState(false); // true = engine calculating

  // helper to convert grid indices to "e4"
  function idxToSquare(fileIdx, rankIdxFromTop) {
    const rankNumber = 8 - rankIdxFromTop;
    const fileLetter = "abcdefgh"[fileIdx];
    return fileLetter + rankNumber;
  }

  function applyMove(from, to) {
    const next = new Chess(game.fen());
    const result = next.move({
      from,
      to,
      promotion: "q",
    });

    if (result) {
      // update chess engine state
      setGame(next);
      setBoardState(next.board());

      // append SAN (like "e4" or "Nf3")
      setMoveHistory(prev => [...prev, result.san]);

      // (future) ask backend here:
      // 1. send FEN -> Flask
      // 2. get bestmove + eval
      // 3. setThinking(false), setEvalScore(serverEval), apply engine move
    }
  }

  function getLegalMovesFrom(square) {
    return game
      .moves({
        square,
        verbose: true,
      })
      .map(m => m.to); // returns ["e4","e5",...]
  }

  function handleSquareClick(squareName) {
    // --- CASE A: nothing selected yet ---
    if (!selectedSquare) {
      const piece = game.get(squareName);
      // must click a piece that exists AND it's your turn color
      if (piece && piece.color === game.turn()) {
        setSelectedSquare(squareName);
        setLegalTargets(getLegalMovesFrom(squareName));
      } else {
        // clicked empty square or opponent piece first -> ignore, keep nothing selected
        setSelectedSquare(null);
        setLegalTargets([]);
      }
      return;
    }

    // --- CASE B: we already had something selected ---
    const from = selectedSquare;
    const isSameSquare = squareName === from;
    const isLegalDest = legalTargets.includes(squareName);

    // B1. Clicked a legal destination -> make the move
    if (isLegalDest) {
      applyMove(from, squareName);
      // applyMove does not clear selection itself, so we do it here:
      setSelectedSquare(null);
      setLegalTargets([]);
      return;
    }

    // B2. Clicked the same square again -> unselect / cancel
    if (isSameSquare) {
      setSelectedSquare(null);
      setLegalTargets([]);
      return;
    }

    // B3. Clicked a *different* square. Possible meanings:
    //  - maybe it's another piece YOU can control -> switch selection to that piece
    //  - otherwise -> clear everything (cancel)

    const piece = game.get(squareName);

    if (piece && piece.color === game.turn()) {
      // switch selection to new piece
      setSelectedSquare(squareName);
      setLegalTargets(getLegalMovesFrom(squareName));
    } else {
      // clicked something irrelevant (empty / enemy not legally capturable from prev selection?)
      // -> clear
      setSelectedSquare(null);
      setLegalTargets([]);
    }
  }


  const squareSizePx = boardPx / 8;

  // BOARD + SIDEBAR LAYOUT
  // On desktop: flex-row (board | sidebar)
  // On mobile: flex-col (board on top, sidebar below)
  return (
    <div className="flex flex-col items-center w-full px-4">
      {/* responsive wrapper */}
      <div className="flex flex-col lg:flex-row gap-10 items-start justify-center">
        {/* ==== BOARD AREA ==== */}
        <div className="flex flex-col items-center">
          {/* fancy frame */}
          {/* <div
            className="rounded-2xl p-[2px] shadow-2xl"
            style={{
              background:
                "linear-gradient(145deg, rgba(80,80,160,0.4) 0%, rgba(10,10,20,0.2) 100%)",
              boxShadow:
                "0 30px 80px rgba(0,0,0,0.9), 0 0 30px rgba(233,69,96,0.15)",
            }}
          > */}
          <div
            className="rounded-2xl p-[2px] shadow-[0_0_40px_rgba(0,255,200,0.2)]"
            style={{
              background:
                "linear-gradient(135deg, rgba(0,255,180,0.15), rgba(0,120,255,0.1))",
              boxShadow:
                "0 0 60px rgba(0,255,180,0.15), 0 0 120px rgba(0,120,255,0.15)",
            }}
          >

            <div
              className="rounded-2xl shadow-inner overflow-hidden"
              style={{
                background:
                  "radial-gradient(circle at 20% 20%, rgba(15,15,30,0.8) 0%, rgba(5,5,10,0.4) 60%, rgba(0,0,0,0) 100%)",
                backdropFilter: "blur(18px)",
                WebkitBackdropFilter: "blur(18px)",
              }}
            >
              {/* board grid */}
              <div
                className="grid relative"
                style={{
                  width: boardPx + "px",
                  height: boardPx + "px",
                  gridTemplateColumns: "repeat(8, 1fr)",
                  gridTemplateRows: "repeat(8, 1fr)",
                }}
              >
                {boardState.map((rankArr, rankIdxFromTop) =>
                  rankArr.map((squareObj, fileIdx) => {
                    const sqName = idxToSquare(fileIdx, rankIdxFromTop);
                    const dark = (fileIdx + rankIdxFromTop) % 2 === 1;
                    const isSelected = selectedSquare === sqName;
                    const isLegalDest = legalTargets.includes(sqName);

                    const baseTileStyle = dark
                      ? {
                          background:
                            "radial-gradient(circle at 30% 30%, rgba(40,40,70,0.8) 0%, rgba(10,10,20,0.8) 60%, rgba(0,0,0,0) 100%)",
                          boxShadow:
                            "inset 0 0 8px rgba(0,0,0,0.8), inset 0 0 30px rgba(0,0,0,0.6)",
                          backdropFilter: "blur(10px)",
                          WebkitBackdropFilter: "blur(10px)",
                        }
                      : {
                          background:
                            "radial-gradient(circle at 30% 30%, rgba(255,255,255,0.35) 0%, rgba(180,180,255,0.08) 60%, rgba(0,0,0,0) 100%)",
                          boxShadow:
                            "inset 0 0 8px rgba(255,255,255,0.4), inset 0 0 30px rgba(255,255,255,0.15)",
                          backdropFilter: "blur(14px)",
                          WebkitBackdropFilter: "blur(14px)",
                        };

                    const ringClass = isSelected
                      ? "ring-2 ring-chess-highlight ring-offset-2 ring-offset-[rgba(0,0,0,0.6)]"
                      : isLegalDest
                      ? "outline outline-2 outline-chess-highlight/70"
                      : "";

                    return (
                      <div
                        key={sqName}
                        onClick={() => handleSquareClick(sqName)}
                        className={[
                          "relative cursor-pointer select-none",
                          ringClass,
                        ].join(" ")}
                        style={{
                          ...baseTileStyle,
                          width: squareSizePx + "px",
                          height: squareSizePx + "px",
                        }}
                      >
                        {/* coords */}
                        {fileIdx === 0 && (
                          <div
                            className="absolute left-1 top-1 text-[10px] text-white/60 font-light"
                            style={{ userSelect: "none" }}
                          >
                            {8 - rankIdxFromTop}
                          </div>
                        )}
                        {rankIdxFromTop === 7 && (
                          <div
                            className="absolute right-1 bottom-1 text-[10px] text-white/60 font-light"
                            style={{ userSelect: "none" }}
                          >
                            {"abcdefgh"[fileIdx]}
                          </div>
                        )}

                        {/* piece */}
                        {squareObj && (
                          <div
                            className="absolute flex items-center justify-center"
                            style={{
                              left: 0,
                              top: 0,
                              width: "100%",
                              height: "100%",
                              fontSize: squareSizePx * 0.7 + "px",
                              lineHeight: 1,
                              color:
                                squareObj.color === "w"
                                  ? "#fff"
                                  : "#000",
                              WebkitTextStroke:
                                squareObj.color === "w"
                                  ? "1px rgba(0,0,0,0.8)"
                                  : "1px rgba(255,255,255,0.8)",
                              filter:
                                squareObj.color === "w"
                                  ? "drop-shadow(0 0 6px rgba(255,255,255,0.5))"
                                  : "drop-shadow(0 0 6px rgba(0,0,0,0.9))",
                              textShadow:
                                squareObj.color === "w"
                                  ? "0 0 10px rgba(255,255,255,0.4)"
                                  : "0 0 10px rgba(0,0,0,0.8)",
                              fontFamily:
                                "'Segoe UI Symbol','Noto Sans Symbols2','Apple Color Emoji','Twemoji Mozilla','sans-serif'",
                              display: "flex",
                              alignItems: "center",
                              justifyContent: "center",
                            }}
                          >
                            {
                              PIECE_UNICODE[
                                squareObj.color === "w"
                                  ? squareObj.type.toUpperCase()
                                  : squareObj.type
                              ]
                            }
                          </div>
                        )}

                        {/* legal dest dot */}
                        {isLegalDest && !squareObj && (
                          <div
                            className="absolute rounded-full bg-chess-highlight/70 shadow-[0_0_15px_rgba(233,69,96,0.9)]"
                            style={{
                              left: "50%",
                              top: "50%",
                              transform:
                                "translate(-50%, -50%)",
                              width: squareSizePx * 0.2 + "px",
                              height: squareSizePx * 0.2 + "px",
                            }}
                          />
                        )}
                      </div>
                    );
                  })
                )}
              </div>
            </div>
          </div>

          {/* turn indicator */}
          <p className="mt-3 text-sm text-slate-300 tracking-wide text-center">
            Turn:{" "}
            <span className="text-chess-highlight font-semibold">
              {game.turn() === "w" ? "White" : "Black"}
            </span>
          </p>
        </div>

        {/* ==== SIDEBAR PANEL ==== */}
        <div
          className="w-full lg:w-[220px] flex-shrink-0"
          style={{
            maxWidth: "220px",
          }}
        >
          <MoveHistory
            history={moveHistory}
            evalScore={evalScore}
            thinking={thinking}
          />
        </div>
      </div>
    </div>
  );
}

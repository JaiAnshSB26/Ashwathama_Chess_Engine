// import { useState, useEffect } from "react";
// import { Chess } from "chess.js";
// import { Chessboard } from "react-chessboard";

// export default function ChessBoardUI() {
//   const [game, setGame] = useState(new Chess());
//   const [boardWidth, setBoardWidth] = useState(400);

//   // responsive board sizing
//   useEffect(() => {
//     function handleResize() {
//       const w = window.innerWidth;
//       if (w < 480) {
//         setBoardWidth(300);
//       } else if (w < 768) {
//         setBoardWidth(340);
//       } else {
//         setBoardWidth(400);
//       }
//     }
//     handleResize();
//     window.addEventListener("resize", handleResize);
//     return () => window.removeEventListener("resize", handleResize);
//   }, []);

//   function safeMutate(fn) {
//     setGame(prev => {
//       const copy = new Chess(prev.fen());
//       fn(copy);
//       return copy;
//     });
//   }

//   function onDrop(sourceSquare, targetSquare) {
//     let move = null;
//     safeMutate(g => {
//       move = g.move({
//         from: sourceSquare,
//         to: targetSquare,
//         promotion: "q",
//       });
//     });

//     if (move === null) return false;
//     return true;
//   }

//   return (
//     <div className="relative flex flex-col items-center justify-center mt-4">
//       {/* outer frame */}
//       <div className="p-[2px] rounded-2xl bg-gradient-to-b from-chess-light/40 to-chess-dark/30 shadow-2xl">
//         <div
//           className="rounded-2xl shadow-inner"
//           style={{
//             background:
//               "linear-gradient(145deg, rgba(20,20,40,0.6) 0%, rgba(10,10,20,0.3) 100%)",
//             backdropFilter: "blur(16px)",
//             WebkitBackdropFilter: "blur(16px)",
//           }}
//         >
//           <Chessboard
//             position={game.fen()}
//             onPieceDrop={onDrop}
//             arePiecesDraggable={true}
//             boardOrientation="white"
//             boardWidth={boardWidth}
//             customBoardStyle={{
//               borderRadius: "1rem",
//               boxShadow: "0 30px 80px rgba(0,0,0,0.9)",
//             }}
//             customLightSquareStyle={{
//               background:
//                 "radial-gradient(circle at 30% 30%, rgba(255,255,255,0.4) 0%, rgba(180,180,255,0.08) 60%, rgba(0,0,0,0) 100%)",
//               backdropFilter: "blur(12px)",
//               WebkitBackdropFilter: "blur(12px)",
//               boxShadow:
//                 "inset 0 0 6px rgba(255,255,255,0.4), inset 0 0 20px rgba(255,255,255,0.15)",
//             }}
//             customDarkSquareStyle={{
//               background:
//                 "radial-gradient(circle at 30% 30%, rgba(40,40,70,0.8) 0%, rgba(15,15,30,0.85) 60%, rgba(0,0,0,0) 100%)",
//               backdropFilter: "blur(8px)",
//               WebkitBackdropFilter: "blur(8px)",
//               boxShadow:
//                 "inset 0 0 6px rgba(0,0,0,0.8), inset 0 0 20px rgba(0,0,0,0.6)",
//             }}
//           />
//         </div>
//       </div>

//       {/* whose turn */}
//       <p className="mt-3 text-sm text-slate-300 tracking-wide">
//         Turn:{" "}
//         <span className="text-chess-highlight font-semibold">
//           {game.turn() === "w" ? "White" : "Black"}
//         </span>
//       </p>
//     </div>
//   );
// }

import { useState, useEffect } from "react";
import { Chess } from "chess.js";

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
  const [boardState, setBoardState] = useState(game.board());
  const [selectedSquare, setSelectedSquare] = useState(null);
  const [legalTargets, setLegalTargets] = useState([]);
  const [boardPx, setBoardPx] = useState(420);

  // resize for viewport fit
  useEffect(() => {
    function handleResize() {
      const vh = window.innerHeight;
      const vw = window.innerWidth;

      // reserve some space for header, turn label, etc.
      const maxByHeight = vh - 200;
      const maxByWidth = vw - 40;

      const size = Math.min(maxByHeight, maxByWidth, 480);
      setBoardPx(size < 280 ? 280 : size);
    }
    handleResize();
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  // helper: square "a1" from indices
  function idxToSquare(fileIdx, rankIdxFromTop) {
    const rankNumber = 8 - rankIdxFromTop;
    const fileLetter = "abcdefgh"[fileIdx];
    return fileLetter + rankNumber;
  }

  function safeCommitMove(from, to) {
    const next = new Chess(game.fen());
    const moveResult = next.move({
      from,
      to,
      promotion: "q",
    });
    if (moveResult) {
      setGame(next);
      setBoardState(next.board());
    }
  }

  function handleSquareClick(squareName) {
    // No square currently selected -> try selecting one
    if (!selectedSquare) {
      const piece = game.get(squareName);
      if (!piece) return;
      if (piece.color !== game.turn()) return;

      const movesFromHere = game.moves({
        square: squareName,
        verbose: true,
      });

      setSelectedSquare(squareName);
      setLegalTargets(movesFromHere.map(m => m.to));
      return;
    }

    // We have a selected square already
    const from = selectedSquare;
    const to = squareName;

    safeCommitMove(from, to);

    // Reset selection
    setSelectedSquare(null);
    setLegalTargets([]);
  }

  // each cell size in px
  const squareSizePx = boardPx / 8;

  return (
    <div className="flex flex-col items-center mt-4">
      {/* outer frame */}
      <div
        className="rounded-2xl p-[2px] shadow-2xl"
        style={{
          background:
            "linear-gradient(145deg, rgba(80,80,160,0.4) 0%, rgba(10,10,20,0.2) 100%)",
          boxShadow:
            "0 30px 80px rgba(0,0,0,0.9), 0 0 30px rgba(233,69,96,0.15)",
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

                // tile visual base
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

                // borders/highlights (ring for selected, outline for legal)
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
                    {/* coordinates in corner */}
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

                    {/* piece, absolutely centered so it doesn't resize the cell */}
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
                            squareObj.color === "w" ? "#fff" : "#000",
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

                    {/* legal move dot (only show if it's a legal dest and empty square) */}
                    {isLegalDest && !squareObj && (
                      <div
                        className="absolute rounded-full bg-chess-highlight/70 shadow-[0_0_15px_rgba(233,69,96,0.9)]"
                        style={{
                          left: "50%",
                          top: "50%",
                          transform: "translate(-50%, -50%)",
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

      {/* Turn indicator */}
      <p className="mt-3 text-sm text-slate-300 tracking-wide">
        Turn:{" "}
        <span className="text-chess-highlight font-semibold">
          {game.turn() === "w" ? "White" : "Black"}
        </span>
      </p>
    </div>
  );
}

import { useState, useEffect, useRef } from "react";
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
  // chess.js game state
  const [game, setGame] = useState(() => new Chess());
  const [boardState, setBoardState] = useState(() => game.board());

  // UCI move history — state + ref (ref = always latest)
  const [uciHistory, setUciHistory] = useState([]);
  const uciRef = useRef([]);

  // UI selection state
  const [selectedSquare, setSelectedSquare] = useState(null);
  const [legalTargets, setLegalTargets] = useState([]);

  // move history (SAN)
  const [moveHistory, setMoveHistory] = useState([]);

  // eval + thinking indicator
  const [evalScore, setEvalScore] = useState(0.0);
  const [thinking, setThinking] = useState(false);

  const [gameStatus, setGameStatus] = useState(""); 
  // e.g. "Checkmate — Black wins", "Stalemate — Draw", "Check!", etc.
  const [engineError, setEngineError] = useState(false);
  const engineErrorRef = useRef(false);


  // responsive board sizing
  const [boardPx, setBoardPx] = useState(420);
  useEffect(() => {
    function handleResize() {
      const vh = window.innerHeight;
      const vw = window.innerWidth;
      const maxByHeight = vh - 220;
      const maxByWidth = vw * 0.6;
      const size = Math.min(maxByHeight * 0.9, maxByWidth, 520);
      setBoardPx(size < 260 ? 260 : size);
    }
    handleResize();
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  // translate indices to squares like "e4"
  function idxToSquare(fileIdx, rankIdxFromTop) {
    const rankNumber = 8 - rankIdxFromTop;
    const fileLetter = "abcdefgh"[fileIdx];
    return fileLetter + rankNumber;
  }

  function updateStatus(chessObj) {
    if (chessObj.isCheckmate()) {
      // if it's mate, the side that JUST moved is the winner.
      // chessObj.turn() is the side *to move now* (the loser).
      const winner = chessObj.turn() === "w" ? "Black" : "White";
      setGameStatus(`Checkmate — ${winner} wins`);
      return;
    }

    if (chessObj.isStalemate()) {
      setGameStatus("Stalemate — Draw");
      return;
    }

    if (chessObj.isDraw()) {
      setGameStatus("Draw");
      return;
    }

    if (chessObj.isCheck()) {
      setGameStatus("Check!");
      return;
    }

    // normal ongoing game
    setGameStatus("");
  }

  // -------------------------
  // Core gameplay logic
  // -------------------------
  function applyMove(from, to) {
    // clone current board
    const afterHuman = new Chess(game.fen());

    let humanResult = null;
    try {
      humanResult = afterHuman.move({
        from,
        to,
        promotion: "q",
      });
    } catch (err) {
      console.warn("[CLIENT] illegal move threw", { from, to }, err);

      // important: unlock UI so user can click something else
      setSelectedSquare(null);
      setLegalTargets([]);
      return;
    }

    if (!humanResult) {
      console.warn("[CLIENT] move rejected as illegal", { from, to });

      // same unlock
      setSelectedSquare(null);
      setLegalTargets([]);
      return;
    }

    // ... keep rest of your logic the same after this point
    const humanMoveUCI =
      humanResult.from + humanResult.to + (humanResult.promotion || "");

    // update visible board + SAN history
    setGame(afterHuman);
    setBoardState(afterHuman.board());
    setMoveHistory((prev) => [...prev, humanResult.san]);

    // ---- Update UCI history (ref + state, synchronous) ----
    const historyAfterHuman = [...uciRef.current, humanMoveUCI];
    uciRef.current = historyAfterHuman;
    setUciHistory(historyAfterHuman);

    console.log("----------------------------------------------------");
    console.log("[HUMAN MOVE APPLIED]");
    console.log("  humanResult.san =", humanResult.san);
    console.log("  humanMoveUCI    =", humanMoveUCI);
    console.log("  updatedUciHistoryAfterHuman =", historyAfterHuman);
    console.log("  FEN afterHuman =", afterHuman.fen());

    // update status text (check, mate, etc.)
    updateStatus(afterHuman);

    // stop if game ended (if game already ended from the human move, stop and don't ask engine)
    if (afterHuman.isGameOver()) {
      console.log("[GAME OVER] after human move");
      setThinking(false);
      setSelectedSquare(null);
      setLegalTargets([]);
      return;
    }

    // clear selection and mark engine thinking
    setThinking(true);
    setSelectedSquare(null);
    setLegalTargets([]);

    const payload = {
      moves: historyAfterHuman,
      movetime: 500,
    };

    console.log("[FETCH -> /move] payload =", payload);

    // earlier (somehow worked before laptop restarted): fetch("http://127.0.0.1.5001/move", {)
    fetch("http://127.0.0.1:5055/move", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    })
      .then((res) => {
        console.log("[ENGINE RESPONSE STATUS]", res.status);
        return res.json();
      })
      .then((data) => {
        console.log("[/move RESPONSE]", data);

        if (!data.bestmove) {
          console.error("[ENGINE ERROR]", data);
          // freeze the game so we don't desync
          setEngineError(true);
          setGameStatus("Engine error — please start a New Game");
          setThinking(false);
          setSelectedSquare(null);
          setLegalTargets([]);
          return;
        }

        const engineMoveUCI = data.bestmove;
        const engineFrom = engineMoveUCI.slice(0, 2);
        const engineTo = engineMoveUCI.slice(2, 4);
        const enginePromotion =
          engineMoveUCI.length > 4 ? engineMoveUCI.slice(4, 5) : undefined;

        const afterEngine = new Chess(afterHuman.fen());
        let engineResult = null;
        try {
          engineResult = afterEngine.move({
            from: engineFrom,
            to: engineTo,
            promotion: enginePromotion || "q",
          });
        } catch (err) {
          console.error("[ENGINE ILLEGAL MOVE THROW?]", engineMoveUCI, err);
          setEngineError(true);
          return;
        }
        if (!engineResult) {
          console.error("[ENGINE ILLEGAL MOVE?]", engineMoveUCI);
          setEngineError(true);
          return;
        }

        // update UI board after engine reply
        setGame(afterEngine);
        setBoardState(afterEngine.board());
        setMoveHistory((prev) => [...prev, engineResult.san]);

        updateStatus(afterEngine);

        if (afterEngine.isGameOver()) {
            // game ended on engine's move, freeze thinking
            setThinking(false);
        }

        // ---- Update UCI history again (ref + state) ----
        const finalHistory = [...historyAfterHuman, engineMoveUCI];
        uciRef.current = finalHistory;
        setUciHistory(finalHistory);

        console.log("[ENGINE MOVE APPLIED]");
        console.log("  engineMoveUCI =", engineMoveUCI);
        console.log("  engineResult.san =", engineResult.san);
        console.log("  finalHistory =", finalHistory);
        console.log("  FEN afterEngine =", afterEngine.fen());

        if (typeof data.eval === "number") {
          setEvalScore(data.eval);
        }

        //Update status after engine move.
        updateStatus(afterEngine);

        //If engine just ended the game.
        if (afterEngine.isGameOver()) {
            console.log("[GAME OVER] after engine move");
            // thinking will also get cleared in finally
            // but we can defensively kill highlights here
            setThinking(false);
            setSelectedSquare(null);
            setLegalTargets([]);
        }
      })
      .catch((err) => {
        console.error("[FETCH ERROR]", err);
        setEngineError(true);
      })
      .finally(() => {
        if (!engineErrorRef.current) {
          setThinking(false);
          setSelectedSquare(null);
          setLegalTargets([]);
        } else {
            setThinking(false);
        }
        console.log("----------------------------------------------------");
      });
  }

  // -------------------------
  // UI click handling
  // -------------------------
  function handleSquareClick(squareName) {
    if (!selectedSquare) {
      const piece = game.get(squareName);
      if (!piece) return;
      if (piece.color !== game.turn()) return;

      const movesFromHere = game.moves({
        square: squareName,
        verbose: true,
      });

      setSelectedSquare(squareName);
      setLegalTargets(movesFromHere.map((m) => m.to));
      return;
    }

    const from = selectedSquare;
    const to = squareName;
    applyMove(from, to);

    setSelectedSquare(null);
    setLegalTargets([]);
  }

  function buildPGN() {
    // figure out result for PGN header + ending
    let resultTag = "*"; // ongoing/unknown default

    if (gameStatus.startsWith("Checkmate")) {
      if (gameStatus.includes("White wins")) {
        resultTag = "1-0";
      } else if (gameStatus.includes("Black wins")) {
        resultTag = "0-1";
      }
    } else if (gameStatus.includes("Draw")) {
      resultTag = "1/2-1/2";
    }

    // headers (minimal for now)
    let pgn = `[Event "Casual Game"]\n`;
    pgn += `[Site "Local"]\n`;
    pgn += `[White "Human"]\n`;
    pgn += `[Black "Engine"]\n`;
    pgn += `[Result "${resultTag}"]\n\n`;

    // moves body like: "1. d4 Nc6 2. f3 e6 ..."
    let body = "";
    for (let i = 0; i < moveHistory.length; i += 2) {
      const moveNum = i / 2 + 1;
      const whiteMove = moveHistory[i] || "";
      const blackMove = moveHistory[i + 1] || "";
      body += `${moveNum}. ${whiteMove} ${blackMove} `;
    }
    body += resultTag;

    pgn += body.trim();
    return pgn;
  }

  function handleDownloadPGN() {
    const pgnText = buildPGN();
    const blob = new Blob([pgnText], { type: "text/plain" });
    const url = URL.createObjectURL(blob);

    const a = document.createElement("a");
    a.href = url;
    a.download = "game.pgn";
    a.click();

    URL.revokeObjectURL(url);
  }

  function resetGame() {
    const fresh = new Chess();

    setGame(fresh);
    setBoardState(fresh.board());

    setMoveHistory([]);
    setUciHistory([]);
    uciRef.current = [];

    setEvalScore(0.0);
    setThinking(false);
    setSelectedSquare(null);
    setLegalTargets([]);

    setGameStatus("");

    setEngineError(false);
  }



  const squareSizePx = boardPx / 8;

  // -------------------------
  // Render UI
  // -------------------------
  return (
    <div className="flex flex-col items-center w-full px-4">
      <div className="flex flex-col lg:flex-row gap-10 items-start justify-center">
        {/* BOARD AREA */}
        <div className="flex flex-col items-center">
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

                        {/* legal move indicator */}
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

          {/* turn + eval */}
          {/* p changed to div here for better adaptability, react would parse div inside p too, but it won't globalize. */}
          <div className="mt-3 text-sm text-slate-300 tracking-wide text-center">
            {gameStatus ? (
                <div className="flex flex-col items-center gap-2">
                  <span className="text-chess-highlight font-semibold">
                    {gameStatus}
                  </span>

                  <button
                    onClick={resetGame}
                    className="text-xs text-slate-400 underline hover:text-slate-200"
                  >
                    New Game
                  </button>
                </div>
            ) : (
                <>
                  Turn:{" "}
                  <span className="text-chess-highlight font-semibold">
                    {game.turn() === "w" ? "White" : "Black"}
                  </span>
                  {thinking && (
                    <span className="ml-2 text-xs text-emerald-300 animate-pulse">
                        (engine thinking…)
                    </span>
                  )}
                </>
            )}

            {engineError && !gameStatus && (
                <div className="mt-2 text-xs text-red-400 text-center">
                    Engine error — please start a{" "}
                    <button
                        onClick={resetGame}
                        className="underline hover:text-red-200"
                    >
                        New Game
                    </button>
                </div>
            )}
          </div>
        </div>

        {/* SIDEBAR */}
        <div
          className="w-full lg:w-[220px] flex-shrink-0"
          style={{ maxWidth: "220px" }}
        >
          <MoveHistory
            history={moveHistory}
            evalScore={evalScore}
            thinking={thinking}
            onDownloadPGN={handleDownloadPGN}
          />
        </div>
      </div>
    </div>
  );
}

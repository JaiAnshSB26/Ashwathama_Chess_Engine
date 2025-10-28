import { useState, useEffect } from "react";
import { Chess } from "chess.js";
import { Chessboard } from "react-chessboard";

export default function ChessBoardUI() {
  const [game, setGame] = useState(new Chess());
  const [boardWidth, setBoardWidth] = useState(420);

  // Resize handler so board adapts to screen width
  useEffect(() => {
    function handleResize() {
      const w = window.innerWidth;
      // if mobile-ish, board ~260px
      if (w < 480) {
        setBoardWidth(260);
      // if tablet-ish, ~335px
      } else if (w < 768) {
        setBoardWidth(335);
      // normal desktop
      } else {
        setBoardWidth(420);
      }
    }

    handleResize(); // run once on mount
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  function safeMutate(fn) {
    setGame(prev => {
      const copy = new Chess(prev.fen());
      fn(copy);
      return copy;
    });
  }

  function onDrop(sourceSquare, targetSquare) {
    let move = null;
    safeMutate(g => {
      move = g.move({
        from: sourceSquare,
        to: targetSquare,
        promotion: "q",
      });
    });

    if (move === null) return false;
    return true;
  }

  return (
    <div className="relative flex flex-col items-center justify-center mt-4">
      {/* frame with glow + glass */}
      <div className="p-[1px] rounded-2xl bg-gradient-to-b from-chess-light/40 to-chess-dark/30 shadow-2xl backdrop-blur-sm">
        <div className="rounded-2xl bg-[rgba(255,255,255,0.05)] backdrop-blur-md shadow-inner">
          <Chessboard
            position={game.fen()}
            onPieceDrop={onDrop}
            boardWidth={boardWidth}
            arePiecesDraggable={true}
            boardOrientation="white"
            customDarkSquareStyle={{
              background:
                "linear-gradient(135deg, rgba(30,30,60,0.8), rgba(10,10,20,0.9))",
              backdropFilter: "blur(8px)",
            }}
            customLightSquareStyle={{
              background:
                "linear-gradient(135deg, rgba(240,240,255,0.3), rgba(150,150,180,0.1))",
              backdropFilter: "blur(12px)",
            }}
            customBoardStyle={{
              borderRadius: "16px",
              boxShadow: "0 0 25px rgba(0,0,0,0.6)",
            }}
          />
        </div>
      </div>

      {/* turn indicator */}
      <p className="mt-3 text-sm text-slate-300 tracking-wide">
        Turn:{" "}
        <span className="text-chess-highlight font-semibold">
          {game.turn() === "w" ? "White" : "Black"}
        </span>
      </p>
    </div>
  );
}

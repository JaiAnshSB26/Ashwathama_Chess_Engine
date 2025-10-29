export default function MoveHistory({ history, evalScore, thinking, onDownloadPGN }) {
  // Note for the newer developers: history is array of SAN strings like ["e4", "e5", "Nf3", "Nc6", ...]
  const rows = [];
  for (let i = 0; i < history.length; i += 2) {
    rows.push({
      moveNumber: i / 2 + 1,
      white: history[i],
      black: history[i + 1] || "",
    });
  }

  return (
    <div className="flex flex-col gap-4 w-full text-slate-200">
      {/* Engine/Eval Box */}
      <div className="rounded-xl border border-white/10 bg-[rgba(15,15,30,0.6)] backdrop-blur-md p-4 shadow-xl">
        <div className="text-xs text-slate-400 uppercase tracking-wide mb-1">
          Engine Eval
        </div>
        <div className="text-lg font-semibold text-chess-highlight leading-none">
          {evalScore > 0 ? `+${evalScore.toFixed(2)}` : evalScore.toFixed(2)}
        </div>

        <div className="mt-2 text-[11px] text-slate-400">
          {thinking
            ? "Engine is thinking..."
            : "Stable position. No calculation pending."}
        </div>
      </div>

      {/* Move History Box */}
      <div className="rounded-xl border border-white/10 bg-[rgba(15,15,30,0.6)] backdrop-blur-md p-4 shadow-xl flex flex-col min-h-[180px] max-h-[240px]">
        <div className="flex items-baseline justify-between">
          <h2 className="text-sm font-semibold text-chess-highlight">
            Move History
          </h2>
          <div className="text-[10px] text-slate-500 font-mono">
            {history.length} moves
          </div>
        </div>

        <div className="mt-2 overflow-y-auto pr-2 text-xs leading-relaxed custom-scroll">
          {rows.length === 0 ? (
            <div className="text-slate-500 italic">No moves yet.</div>
          ) : (
            rows.map((row) => (
              <div
                key={row.moveNumber}
                className="grid grid-cols-[auto_1fr_1fr] gap-x-2 py-[2px]"
              >
                <div className="text-slate-400 tabular-nums">
                  {row.moveNumber}.
                </div>
                <div className="text-white">{row.white}</div>
                <div className="text-white">{row.black}</div>
              </div>
            ))
          )}
        </div>
      </div>

      {/* Real PGN Stuff now */}
      <button
        onClick={onDownloadPGN}
        className="w-full py-3 rounded-xl bg-[rgba(40,0,20,0.4)]
                   text-pink-200 text-sm font-medium tracking-wide
                   border border-pink-500/30 shadow-[0_0_20px_rgba(255,0,90,0.4)]
                   hover:bg-[rgba(60,0,30,0.6)] transition-colors"
        disabled={history.length === 0}
      >
        ⬇ Download PGN
      </button>

      {/* Placeholder for Elo */}
      <div className="text-[10px] text-slate-500 text-center">
        Ashwathama Engine • Elo: TBD
      </div>
    </div>
  );
}
import ChessBoardUI from "./components/ChessBoardUI.jsx";
import "./App.css";

export default function App() {
  return (
    <div
      className="
        min-h-screen
        text-white
        flex flex-col items-center
        overflow-x-hidden
        overflow-y-auto
        pb-16 pt-10
        bg-gradient-to-b
        from-[#050509] via-[#0a1220] to-[#001b24]
      "
    >
      {/* HEADER */}
      <div
        className="
          w-full
          max-w-[1400px]
          px-6
          flex flex-col items-center
          text-center
          mb-8
        "
      >
        <div
          className="
            w-full
            max-w-[1500px]
            rounded-xl
            border border-white/10
            bg-[rgba(0,0,0,0.4)]
            backdrop-blur-xl
            px-6 py-6
            shadow-[0_0_40px_rgba(0,255,180,0.08)]
          "
        >
          <h1
            className="
              text-4xl font-bold
              bg-gradient-to-r from-[#ff4f7a] via-[#ff7a95] to-[#ff4f7a]
              bg-clip-text text-transparent
              drop-shadow-[0_0_15px_rgba(255,80,120,0.5)]
            "
          >
            Ashwathama Chess Engine
          </h1>

          <p className="text-slate-300 text-sm mt-3">
            Professional UCI Chess Engine with Modern Web Interface
          </p>
        </div>
      </div>

      {/* MAIN PLAY AREA */}
      <div
        className="
          w-full
          flex
          justify-center
          px-6
        "
      >
        <div
          className="
            play-shell
            w-full
            max-w-[1150px]
            p-8
            flex
            justify-center
          "
        >
          {/* We keep ChessBoardUI exactly as you built it (board + sidebar). */}
          <ChessBoardUI />
        </div>
      </div>

      {/* FOOTER NOTE */}
      <div className="mt-10 text-[10px] text-slate-600 text-center">
        © Ashwathama Engine — experimental build
      </div>
    </div>
  );
}

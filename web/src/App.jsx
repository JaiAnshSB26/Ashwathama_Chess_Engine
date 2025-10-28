import { useState } from 'react'
import './App.css'
import ChessBoardUI from "./components/ChessBoardUI.jsx";

// export default function App() {
//   return (
//     <div className="min-h-screen bg-gradient-to-b from-chess-dark via-chess-accent to-chess-light text-white flex flex-col items-center py-1">
//       <div className="bg-glass/30 border border-white/10 rounded-xl p-5 max-w-4xl w-[90%] shadow-2xl text-center">
//         <h1 className="text-4xl font-bold text-chess-highlight mb-2">
//           Ashwathama Chess Engine
//         </h1>
//         <p className="text-slate-300 mb-4">
//           Professional UCI Chess Engine with Modern Web Interface
//         </p>
//       </div>

//       <ChessBoardUI />
//     </div>
//   );
// }
export default function App() {
  return (
    <div className="h-screen bg-gradient-to-b from-chess-dark via-chess-accent to-chess-light text-white flex flex-col items-center justify-start overflow-hidden pt-6">
      <div className="bg-glass/30 border border-white/10 rounded-xl p-6 max-w-4xl w-[90%] shadow-2xl text-center mb-4">
        <h1 className="text-4xl font-bold text-chess-highlight mb-2">
          Ashwathama Chess Engine
        </h1>
        <p className="text-slate-300 mb-2 text-sm">
          Professional UCI Chess Engine with Modern Web Interface
        </p>
      </div>

      <ChessBoardUI />
    </div>
  );
}

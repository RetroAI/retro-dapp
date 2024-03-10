/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./HomePage.css";

import React, { useEffect, useRef } from "react";

import RetroEngineModule, {
  MainModule,
  RetroEngine,
} from "../wasm/retro_engine.js";

// Allow the TypeScript compiler to recognize the module overrides
declare module "../wasm/retro_engine.js" {
  export default function RetroEngineModule(moduleArg?: {
    locateFile?: (path: string) => string;
  }): Promise<MainModule>;
}

const HomePage: React.FC = () => {
  // Reference to the canvas element
  const canvasRef = useRef<HTMLCanvasElement | null>(null);

  useEffect(() => {
    // Function to update the canvas size to cover the full screen
    const updateCanvasSize = (): void => {
      const canvas: HTMLCanvasElement | null = canvasRef.current;
      if (canvas) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
      }
    };

    // Initial setup for fullscreen canvas
    updateCanvasSize();

    // Ensure canvas size adjusts on window resize
    window.addEventListener("resize", updateCanvasSize);

    let engineInstance: RetroEngine | null = null;

    async function initializeAndUseRetroEngine(): Promise<void> {
      // Provide the locateFile function when initializing your WASM module
      const moduleOverrides: {
        locateFile: (file: string) => string;
      } = {
        locateFile: (file: string) => `/wasm/${file}`,
      };

      // Await the promise to resolve to the module instance
      const wasmModule: MainModule = await RetroEngineModule(moduleOverrides);

      // Instantiate RetroEngine
      engineInstance = new wasmModule.RetroEngine();

      // Initialize RetroEngine
      const initialized: boolean = engineInstance.initialize();
      if (initialized) {
        console.log("RetroEngine initialized successfully");
      } else {
        console.error("Failed to initialize RetroEngine");
      }
    }

    // Call the function to initialize and use the RetroEngine
    initializeAndUseRetroEngine().catch(console.error);

    return (): void => {
      // Cleanup on component unmount
      window.removeEventListener("resize", updateCanvasSize);

      // Deinitialize engine if it has been initialized
      engineInstance?.deinitialize();
    };
  }, []);

  // Render the canvas element
  return (
    <div className="homePageContainer">
      <canvas ref={canvasRef} id="retroEngine" />;
    </div>
  );
};

export default HomePage;

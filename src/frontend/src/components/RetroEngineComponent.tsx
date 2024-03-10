/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./RetroEngineComponent.css";

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

const RetroEngineComponent: React.FC = () => {
  // Reference to the canvas element
  const canvasRef: React.RefObject<HTMLCanvasElement> =
    useRef<HTMLCanvasElement | null>(null);
  const containerRef: React.RefObject<HTMLDivElement> =
    useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    // Function to update the canvas size to cover the full screen
    const updateCanvasSize = (): void => {
      const canvas: HTMLCanvasElement | null = canvasRef.current;
      const container: HTMLDivElement | null = containerRef.current;
      if (canvas && container) {
        canvas.width = container.offsetWidth;
        canvas.height = container.offsetHeight;
      }
    };

    // Initial setup for full size canvas
    updateCanvasSize();

    // Set up ResizeObserver to adjust canvas size to the size of the container div
    const observer: ResizeObserver = new ResizeObserver(updateCanvasSize);
    if (containerRef.current) {
      observer.observe(containerRef.current);
    }

    let engineInstance: RetroEngine | null = null;

    async function initializeRetroEngine(): Promise<void> {
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
    initializeRetroEngine().catch(console.error);

    return (): void => {
      // Cleanup on component unmount
      window.removeEventListener("resize", updateCanvasSize);

      // Deinitialize engine if it has been initialized
      engineInstance?.deinitialize();
      engineInstance?.delete();
    };
  }, []);

  // Render the canvas element
  return (
    <div ref={containerRef} className="canvasContainer">
      <canvas ref={canvasRef} id="retroEngine" />
    </div>
  );
};

export default RetroEngineComponent;

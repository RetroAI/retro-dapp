/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./HomePage.css";

import React, { useEffect, useRef } from "react";

// Rendering parameters
const sideLength: number = 256; // Side length of the square
const fps: number = 60; // Desired frames per second
const interval: number = 1000 / fps; // Interval between frames in milliseconds

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

    // Main drawing function
    const draw = (): void => {
      const canvas: HTMLCanvasElement | null = canvasRef.current;
      if (!canvas) {
        return;
      }

      const ctx: CanvasRenderingContext2D | null = canvas.getContext("2d");
      if (!ctx) {
        return;
      }

      // Current angle for rotation
      let angle: number = 0;

      // Function to animate the square
      const animate = (): void => {
        setTimeout(() => {
          if (!ctx || !canvas) {
            return;
          }

          // Clear the canvas
          ctx.fillStyle = "#000000"; // Set background color to black
          ctx.clearRect(0, 0, canvas.width, canvas.height);

          // Save the current drawing context
          ctx.save();

          // Move to the center of the canvas
          ctx.translate(canvas.width / 2, canvas.height / 2);

          // Rotate the drawing context
          ctx.rotate(angle * (Math.PI / 180));

          // Draw the square
          ctx.fillStyle = "#00FF00"; // Set square color to green
          ctx.fillRect(
            -sideLength / 2,
            -sideLength / 2,
            sideLength,
            sideLength,
          );

          // Restore the drawing context
          ctx.restore();

          // Increment the angle for the next frame
          angle += 2;

          // Reset angle after a full rotation
          if (angle >= 360) {
            angle = 0;
          }

          requestAnimationFrame(animate);
        }, interval);
      };

      animate(); // Start the animation
    };

    // Call the draw function to start everything
    draw();

    return (): void => {
      // Cleanup on component unmount
      window.removeEventListener("resize", updateCanvasSize);
    };
  }, []);

  // Render the canvas element
  return (
    <div className="homePageContainer">
      <canvas ref={canvasRef} />
    </div>
  );
};

export default HomePage;

/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./App.css";

import React from "react";

import RetroEngineComponent from "./components/RetroEngineComponent";

const App: React.FC = () => {
  return (
    <div className="canvasContainer">
      <RetroEngineComponent />
    </div>
  );
};

export default App;

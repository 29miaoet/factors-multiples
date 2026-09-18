import createModule from "./divisor_geography.mjs";

const Module = await createModule();

let game = Module._game_create();

const root = document.createElement("main");
const title = document.createElement("h1");

const layout = document.createElement("div");
const boardPanel = document.createElement("section");
const board = document.createElement("div");

const dialog = document.createElement("section");
const result = document.createElement("h2");
const status = document.createElement("p");
const position = document.createElement("p");
const turn = document.createElement("p");

const controls = document.createElement("div");
const modeLabel = document.createElement("label");
const modeSelect = document.createElement("select");

const firstLabel = document.createElement("label");
const firstSelect = document.createElement("select");

const movesTitle = document.createElement("h3");
const movesList = document.createElement("ol");

const resetButton = document.createElement("button");

const buttons = new Map();
const moves = [];

let sides = ["human", "computer"];

title.textContent = "Factors & Multiples";

layout.style.display = "grid";
layout.style.gridTemplateColumns = "minmax(0, 1fr) 320px";
layout.style.gap = "24px";
layout.style.alignItems = "start";

boardPanel.style.minWidth = "0";

board.style.display = "grid";
board.style.gridTemplateColumns = "repeat(10, 1fr)";
board.style.maxWidth = "600px";
board.style.gap = "4px";

dialog.style.padding = "20px";
dialog.style.border = "1px solid #ccc";
dialog.style.borderRadius = "8px";
dialog.style.background = "#f5f5f5";

result.style.marginTop = "0";

controls.style.display = "grid";
controls.style.gap = "10px";
controls.style.marginBottom = "20px";

modeLabel.textContent = "Game mode";
modeLabel.htmlFor = "game-mode";

modeSelect.id = "game-mode";

const playerVsComputer = document.createElement("option");
playerVsComputer.value = "pvc";
playerVsComputer.textContent = "Player vs Computer";

const computerVsComputer = document.createElement("option");
computerVsComputer.value = "cvc";
computerVsComputer.textContent = "Computer vs Computer";

const playerVsPlayer = document.createElement("option");
playerVsPlayer.value = "pvp";
playerVsPlayer.textContent = "Player vs Player";

modeSelect.append(
    playerVsComputer,
    computerVsComputer,
    playerVsPlayer
);

firstLabel.textContent = "Who goes first";
firstLabel.htmlFor = "first-player";

firstSelect.id = "first-player";

movesTitle.textContent = "Moves";

resetButton.textContent = "New Game";
resetButton.type = "button";

controls.append(
    modeLabel,
    modeSelect,
    firstLabel,
    firstSelect
);

dialog.append(
    result,
    status,
    position,
    turn,
    controls,
    movesTitle,
    movesList,
    resetButton
);

boardPanel.append(board);
layout.append(boardPanel, dialog);

root.append(title, layout);

document.body.append(root);

for (let number = 1; number <= 100; number++) {
    const button = document.createElement("button");

    button.textContent = number;
    button.type = "button";

    button.addEventListener("click", () => {
        play(number);
    });

    buttons.set(number, button);
    board.append(button);
}

function isAdjacent(a, b) {
    return a !== b &&
        (a % b === 0 || b % a === 0);
}

function isLegalMove(number) {
    if (!Module._game_is_available(game, number)) {
        return false;
    }

    const lastMove = Module._game_last_move(game);

    if (lastMove === -1) {
        return number >= 2 &&
            number < 50 &&
            number % 2 === 0;
    }

    return isAdjacent(lastMove, number);
}

function getLegalMoves() {
    const result = [];

    for (let number = 1; number <= 100; number++) {
        if (isLegalMove(number)) {
            result.push(number);
        }
    }

    return result;
}

function getBestMove() {
    return Module._game_best_move(game);
}

function getCurrentSide() {
    return Module._game_turn(game);
}

function getCurrentActor() {
    return sides[getCurrentSide()];
}

function getSideName(side) {
    const mode = modeSelect.value;

    if (mode === "pvp") {
        return `Player ${side + 1}`;
    }

    if (mode === "cvc") {
        return `Computer ${side + 1}`;
    }

    return sides[side] === "human"
        ? "You"
        : "Computer";
}

function addMove(number, side, actor) {
    moves.push({
        number,
        side,
        actor
    });

    updateMoveList();
}

function updateMoveList() {
    movesList.replaceChildren();

    for (let i = 0; i < moves.length; i += 2) {
        const item = document.createElement("li");

        const firstMove = moves[i];
        const secondMove = moves[i + 1];

        item.textContent =
            secondMove === undefined
                ? `${firstMove.number}, …`
                : `${firstMove.number}, ${secondMove.number}`;

        movesList.append(item);
    }
}

function updateFirstPlayerOptions() {
    const mode = modeSelect.value;

    firstSelect.replaceChildren();

    if (mode === "pvc") {
        const player = document.createElement("option");
        player.value = "human";
        player.textContent = "Player";

        const computer = document.createElement("option");
        computer.value = "computer";
        computer.textContent = "Computer";

        firstSelect.append(player, computer);
    } else if (mode === "cvc") {
        const computer1 = document.createElement("option");
        computer1.value = "computer1";
        computer1.textContent = "Computer 1";

        const computer2 = document.createElement("option");
        computer2.value = "computer2";
        computer2.textContent = "Computer 2";

        firstSelect.append(computer1, computer2);
    } else {
        const player1 = document.createElement("option");
        player1.value = "player1";
        player1.textContent = "Player 1";

        const player2 = document.createElement("option");
        player2.value = "player2";
        player2.textContent = "Player 2";

        firstSelect.append(player1, player2);
    }
}

function configureSides() {
    const mode = modeSelect.value;
    const first = firstSelect.value;

    if (mode === "pvc") {
        if (first === "human") {
            sides = ["human", "computer"];
        } else {
            sides = ["computer", "human"];
        }

        return;
    }

    if (mode === "cvc") {
        sides = ["computer", "computer"];
        return;
    }

    sides = ["human", "human"];
}

function updateBoard() {
    const lastMove = Module._game_last_move(game);
    const currentActor = getCurrentActor();
    const legalMoves = new Set(getLegalMoves());

    for (let number = 1; number <= 100; number++) {
        const button = buttons.get(number);

        const available =
            Module._game_is_available(game, number) !== 0;

        button.disabled =
            !available ||
            currentActor !== "human" ||
            !legalMoves.has(number);

        button.style.fontWeight =
            number === lastMove
                ? "bold"
                : "";
    }

    /*
     * Reset styles first, then style squares that have actually
     * been played.
     */
    for (const button of buttons.values()) {
        button.style.background = "";
        button.style.color = "";
        button.style.border = "";
    }

    for (const move of moves) {
        const button = buttons.get(move.number);

        if (move.actor === "human") {
            button.style.background = "#4caf50";
            button.style.color = "white";
            button.style.border = "2px solid #2e7d32";
        } else {
            button.style.background = "#e57373";
            button.style.color = "white";
            button.style.border = "2px solid #c62828";
        }
    }

    /*
     * Keep the most recent move visually emphasized.
     */
    if (lastMove !== -1) {
        const lastButton = buttons.get(lastMove);
        lastButton.style.fontWeight = "bold";
    }
}

function updateDialog() {
    const currentSide = getCurrentSide();
    const currentActor = getCurrentActor();
    const currentSideName = getSideName(currentSide);
    const bestMove = getBestMove();

    if (bestMove === -1) {
        const winnerSide = currentSide === 0 ? 1 : 0;
        const winner = getSideName(winnerSide);

        result.textContent = `${winner} wins`;
        status.textContent =
            `${currentSideName} has no legal move.`;

        position.textContent =
            "Position: losing";

        turn.textContent =
            "Game over.";

        return;
    }

    result.textContent =
        `${currentSideName}'s turn`;

    status.textContent =
        currentActor === "human"
            ? "Choose a legal move."
            : "Computer is thinking...";

    position.textContent =
        `Position: winning · Best move: ${bestMove}`;

    turn.textContent =
        `Turn: ${currentSideName}`;
}

function updateUI() {
    updateBoard();
    updateDialog();
    updateMoveList();
}

function makeMove(number) {
    const side = getCurrentSide();
    const actor = getCurrentActor();

    const success =
        Module._game_make_move(game, number);

    if (!success) {
        return false;
    }

    addMove(number, side, actor);
    updateUI();

    return true;
}

function play(number) {
    if (getCurrentActor() !== "human") {
        return;
    }

    if (!isLegalMove(number)) {
        return;
    }

    if (!makeMove(number)) {
        return;
    }

    runComputerTurns();
}

function runComputerTurns() {
    while (getCurrentActor() === "computer") {
        const computerMove = getBestMove();

        /*
         * -1 means that the current position is losing,
         * so there is no move to make.
         */
        if (computerMove === -1) {
            break;
        }

        if (!makeMove(computerMove)) {
            break;
        }
    }

    updateUI();
}

function reset() {
    Module._game_destroy(game);
    game = Module._game_create();

    moves.length = 0;

    configureSides();
    updateUI();
    runComputerTurns();
}

modeSelect.addEventListener("change", () => {
    updateFirstPlayerOptions();
    reset();
});

firstSelect.addEventListener("change", () => {
    reset();
});

resetButton.addEventListener("click", reset);

updateFirstPlayerOptions();
configureSides();
updateUI();
runComputerTurns();


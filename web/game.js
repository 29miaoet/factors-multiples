import createModule from "./divisor_geography.mjs";

const Module = await createModule();

let game = Module._game_create();

const root = document.createElement("main");
const title = document.createElement("h1");
const status = document.createElement("p");
const board = document.createElement("div");
const resetButton = document.createElement("button");

title.textContent = "Divisor Geography";

board.style.display = "grid";
board.style.gridTemplateColumns = "repeat(10, 1fr)";
board.style.maxWidth = "500px";
board.style.gap = "4px";

resetButton.textContent = "New Game";

root.append(
    title,
    status,
    board,
    resetButton
);

document.body.append(root);

const buttons = new Map();

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
    return a !== b && (a % b === 0 || b % a === 0);
}

function legalHumanMove(number) {
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

function getHumanLegalMoves() {
    const result = [];

    for (let number = 1; number <= 100; number++) {
        if (legalHumanMove(number)) {
            result.push(number);
        }
    }

    return result;
}

function updateBoard() {
    const lastMove = Module._game_last_move(game);
    const turn = Module._game_turn(game);

    const humanMoves = new Set(
        turn === 0
            ? getHumanLegalMoves()
            : []
    );

    for (let number = 1; number <= 100; number++) {
        const button = buttons.get(number);
        const available =
            Module._game_is_available(game, number) !== 0;

        button.disabled =
            !available ||
            turn !== 0 ||
            !humanMoves.has(number);

        button.style.fontWeight =
            number === lastMove
                ? "bold"
                : "";
    }

    if (turn === 0) {
        if (humanMoves.size === 0) {
            status.textContent =
                "You have no legal move. Computer wins.";
        } else if (lastMove === -1) {
            status.textContent =
                "Your turn: choose an even number from 2 to 48.";
        } else {
            status.textContent =
                `Your turn: choose a factor or multiple of ${lastMove}.`;
        }
    } else {
        status.textContent = "Computer is thinking...";
    }
}

function play(number) {
    if (!legalHumanMove(number)) {
        return;
    }

    const success =
        Module._game_make_move(game, number);

    if (!success) {
        return;
    }

    updateBoard();

    // It is now the computer's turn.
    const computerMove =
        Module._game_best_move(game);

    if (computerMove === -1) {
        updateBoard();
        status.textContent =
            "Computer has no legal move. You win.";
        return;
    }

    Module._game_make_move(
        game,
        computerMove
    );

    updateBoard();

    if (getHumanLegalMoves().length === 0) {
        status.textContent =
            `Computer chose ${computerMove}. You have no legal move. Computer wins.`;
    }
}

function reset() {
    Module._game_destroy(game);
    game = Module._game_create();
    updateBoard();
}

resetButton.addEventListener("click", reset);

updateBoard();

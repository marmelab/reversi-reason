[%bs.raw {|require('./index.css')|}];

open Player;

let blackPlayer = { name: "White Player", color: White };
let whitePlayer = { name: "Black Player", color: Black };

let game: Game.game = {
    board: Board.init(8, 8),
    players: [|blackPlayer, whitePlayer|],
    playerIndex: 0,
    finished: false
};

ReactDOMRe.renderToElementWithId(<App game />, "root");


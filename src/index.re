[%bs.raw {|require('./index.css')|}];

open Player;

let blackPlayer = { name: "White Player", color: White };
let whitePlayer = { name: "Black Player", color: Black };

let game = Game.init([|blackPlayer, whitePlayer|]);

ReactDOMRe.renderToElementWithId(<App game />, "root");


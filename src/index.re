[%bs.raw {|require('./index.css')|}];

let game = Game.init([|
    Player.{ name: "White Player", color: White },
    Player.{ name: "Black Player", color: Black }
|]);

ReactDOMRe.renderToElementWithId(<App game />, "root");


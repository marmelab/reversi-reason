type player = {
    name: string,
    color: Cell.color
};

let bullet = player => switch(player.color) {
    | Cell.Black => {js|•|js}
    | Cell.White => {js|○|js}
};

let component = ReasonReact.statelessComponent("Player");

let make = (~player, ~current, _children) => {
    ...component,
    render: (_self) => 
        <div className=("player" ++ (current ? " current" : ""))>
            (ReasonReact.stringToElement(player.name))
            (ReasonReact.stringToElement(" " ++ bullet(player)))
        </div>
};

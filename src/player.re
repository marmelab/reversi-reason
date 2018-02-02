open ReasonReact;

type player = {
    name: string,
    color: Cell.color
};

let bullet = ({ color }) => color == Cell.Black ? {js|•|js} : {js|○|js};

let component = statelessComponent("Player");

let make = (~player, ~current, _children) => {
    ...component,
    render: (_self) => 
        <div className=("player" ++ (current ? " current" : ""))>
            (stringToElement(player.name))
            (stringToElement(" " ++ bullet(player)))
        </div>
};

type color = White | Black;

type cell = {
    x: int,
    y: int,
    color: option(color),
};

let component = ReasonReact.statelessComponent("Cell");

let make = (~color, ~onClick, _children) => {
    ...component,
    render: (_self) => {
        let identifier = switch (color) {
            | Some(Black) => "black"
            | Some(White) => "white"
            | _ => ""
        };

        <div className="cell">
            <div className=("disk " ++ identifier) onClick />
        </div>
    }
};

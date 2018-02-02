let component = ReasonReact.statelessComponent("App");

let make = (~game, _children) => {
  ...component,
  render: (_self) =>
    <div className="root">
        <Game game />
    </div>
};

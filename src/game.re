open ReasonReact;

type game = {
    board: Board.board,
    players: array(Player.player),
    playerIndex: int,
    finished: bool
};

exception CantSwitchPlayer(game);
exception InsolubleGame(game);

let currentPlayer = game => game.players[game.playerIndex];

let init = players => {
    board: Board.init(8, 8),
    players,
    playerIndex: 0,
    finished: false
};

let playerCanPlay = (game, playerIndex) => {
    let color = game.players[playerIndex].color;

    Board.availableCellChanges(game.board, Some(color))
        |> Array.length > 0
};

let switchPlayer = game => {
    let { playerIndex } = game;
    let nextPlayerIndex = playerIndex == 1 ? 0 : 1;

    switch (playerCanPlay(game, nextPlayerIndex)) {
    | true => { ...game, playerIndex: nextPlayerIndex }
    | false => switch(playerCanPlay(game, playerIndex)) {
        | true => raise(CantSwitchPlayer(game))
        | false => raise(InsolubleGame({ ...game, finished: true }))
    }
    }
};

let applyCellClick = (game, x, y) => Board.({
    let color = Some(currentPlayer(game).color);
    let board = applyCellChange(game.board, { x, y, color });

    switch(getCountForColor(board, None)) {
    | 0 => { ...game, finished: true }
    | _ => switchPlayer({ ...game, board })
    }
});

let locationReload = (_) => [%bs.raw {| window.location.reload() |}];

type action = Click(int, int) | Restart;

type state = { game, message: option(string) };

let reducer = (action, state) => switch (action) {
    | Click(x, y) => switch(applyCellClick(state.game, x, y)) {
        | game => Update({
            game,
            message: None
        })
        | exception Board.InvalidMove => Update({
            ...state,
            message: Some("Invalid Move")
        })
        | exception InsolubleGame(game) => Update({
            game,
            message: Some("No One Can Play")
        })
        | exception CantSwitchPlayer(game) => Update({
            game,
            message: Some("Opponent can't play, play again!")
        })
    }
    | Restart => SideEffects(locationReload)
};

let component = reducerComponent("Game");

let make = (~game, _children) => {
  ...component,
  initialState: () => { game, message: None },
  reducer,
  render: (self) => {
    let { game, message } = self.state;

    let messageElement = switch(message) {
        | None => nullElement
        | Some(message) => stringToElement(message)
    };

    <div className="game">
        (messageElement)
        <Board
            board=game.board
            onCellClick={(x, y) => self.send(Click(x, y))}
        />
        <div className="players">
            (arrayToElement(
                Array.mapi((i, player) => (
                    <Player
                        player
                        key=string_of_int(i)
                        current=(game.playerIndex == i)
                    />
                ), game.players)
            ))
        </div>
        (game.finished ?
            <div className="finished">
                (stringToElement("Game Over"))<br />
                <button onClick={(_) => self.send(Restart)}>
                    (stringToElement("Start Another Game"))
                </button>
            </div>
        : nullElement)
    </div>
  }
};

open Jest;
open Enzyme;
open Expect;

configureEnzyme(react_16_adapter());

let matrixToCells = matrix => Array.(
    mapi((y, row) => mapi((x, color) => (Cell.{ x, y, color }), row), matrix)
        |> fold_left(append, [||])
        |> to_list
        |> Board.sortCells
);

let (w, b, n) = (Some(Cell.White), Some(Cell.Black), None);

let fakeGame = Game.init([|
    { name: "foo", color: Cell.Black },
    { name: "bar", color: Cell.White },
|]);

describe("Game", () => {
    describe("currentPlayer", () => {
        test("should return current player for game", () => {
            expect(Game.currentPlayer(fakeGame))
                |> toEqual(Player.{ name: "foo", color: Cell.Black })
        });
    });

    describe("playerCanPlay", () => {
        test("should return true if player can place cell change", () => {
            expect(Game.playerCanPlay(fakeGame, 0)) |> toBe(true)
        });

        test("should return false if player can't place cell change", () => {
            expect(Game.playerCanPlay({
                ...fakeGame,
                board: Board.init(2, 2)
            }, 0)) |> toBe(false)
        });
    });

    describe("switchPlayer", () => {
        test("should return a switched game if player can play", () => {
            let originPlayerIndex = fakeGame.playerIndex;

            expect(Game.switchPlayer(fakeGame).playerIndex)
                |> not_
                |> toBe(originPlayerIndex)
        });

        test("should raise CantSwitchPlayer if player can't be switched", () => {
            let gameWithoutMove = {
                ...fakeGame,
                board: { width: 4, height: 4, cells: matrixToCells([|
                    [|b, w, n, n|],
                    [|n, w, n, n|],
                    [|n, n, n, n|],
                    [|n, n, n, n|]
                |]) }
            };

            expect(() => Game.switchPlayer(gameWithoutMove))
                |> toThrowException(Game.CantSwitchPlayer(gameWithoutMove))
        });

        test("should raise InsolubleGame with finished game if there's no possible cell change", () => {
            let gameWithoutSolution = {
                ...fakeGame,
                board: Board.init(2, 2)
            };

            expect(() => Game.switchPlayer(gameWithoutSolution))
                |> toThrowException(Game.InsolubleGame({
                    ...gameWithoutSolution,
                    finished: true
                }))
        });
    });

    describe("applyCellClick", () => {
        test("should return finished game if last cell change", () => {
            let game = {
                ...fakeGame,
                board: { ...Board.init(3, 3), cells: matrixToCells([|
                    [|w, w, w|],
                    [|n, w, b|],
                    [|w, w, w|]
                |]) }
            };

            expect(Game.applyCellClick(game, 0, 1)) |> toEqual({
                ...game,
                finished: true
            })
        });

        test("should return game with switched player instead", () => {
            let game = {
                ...fakeGame,
                board: { ...Board.init(4, 4), cells: matrixToCells([|
                    [|n, n, n, n|],
                    [|n, b, w, n|],
                    [|n, w, b, n|],
                    [|n, n, n, n|]
                |]) }
            };

            expect(Game.applyCellClick(game, 1, 3).playerIndex) |> toEqual(1)
        });
    });

    describe("reducer", () => {
        describe("should for Restart action", () => {
            test("should return a SideEffects with window.location.reload", () => {
                expect(Game.reducer(Restart, { game: fakeGame, message: None }))
                    |> toEqual(ReasonReact.SideEffects(Game.locationReload))
            });
        });

        describe("should for Click action", () => {
            test("return an Update with None as message if game is updated", () => Game.({
                expect(reducer(Click(3, 5), { game: fakeGame, message: Some("foo") }))
                    |> toEqual(ReasonReact.Update({
                        game: Game.applyCellClick(fakeGame, 3, 5),
                        message: None
                    }))
            }));

            test("return an Update with 'Invalid Move' as message if InvalidMove is raised", () => Game.({
                expect(reducer(Click(0, 0), { game: fakeGame, message: None }))
                |> toEqual(ReasonReact.Update({
                    game: fakeGame,
                    message: Some("Invalid Move")
                }))
            }));

            test("return an Update with 'No One Can Play' as message if InsolubleGame is raised", () => Game.({
                let board = Board.init(4, 4);
                
                let game = {
                    ...fakeGame,
                    board:  { ...board, cells: matrixToCells([|
                        [|n, n, n, n|],
                        [|n, b, w, n|],
                        [|n, b, b, n|],
                        [|n, n, n, n|],
                    |]) }
                };

                expect(reducer(Click(2, 0), { game, message: None }))
                    |> toEqual(ReasonReact.Update({
                        game: {
                            ...game,
                            finished: true,
                            board: { ...board, cells: matrixToCells([|
                                [|n, n, b, n|],
                                [|n, b, b, n|],
                                [|n, b, b, n|],
                                [|n, n, n, n|],
                            |]) }
                        },
                        message: Some("No One Can Play")
                    }))
            }));

            test("return an Update with 'Opponent can't play, play again!' as message if CantSwitchPlayer is raised", () => Game.({
                let board = Board.init(4, 4);
                
                let game = {
                    ...fakeGame,
                    board:  { ...board, cells: matrixToCells([|
                        [|b, w, n, n|],
                        [|n, w, n, n|],
                        [|n, n, n, n|],
                        [|n, n, n, n|],
                    |]) }
                };

                expect(reducer(Click(2, 0), { game, message: None }))
                    |> toEqual(ReasonReact.Update({
                        game: {
                            ...game,
                            board: { ...board, cells: matrixToCells([|
                                [|b, b, b, n|],
                                [|n, w, n, n|],
                                [|n, n, n, n|],
                                [|n, n, n, n|],
                            |]) }
                        },
                        message: Some("Opponent can't play, play again!")
                    })) 
            }));
        });
    });

    describe("<Game />", () => {
        test("should render a <Board />", () => {
            let wrapper = shallow(<Game game=fakeGame />);
            expect(wrapper |> find("Board") |> length) |> toBe(1);
        });

        test("should render <Player(s) />", () => {
            let wrapper = shallow(<Game game=fakeGame />);
            expect(wrapper |> find("Player") |> length) |> toBe(2);
        });

        test("should render 'finished' block if game is finished", () => {
            let wrapper = shallow(
                <Game game={ ...fakeGame, finished: true } />
            );

            expect(wrapper |> find(".finished") |> length) |> toBe(1);
        });
    });
});
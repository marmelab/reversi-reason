open Jest;
open Enzyme;
open Expect;

let matrixToCells = matrix => Array.(
    mapi((y, row) => mapi((x, color) => (Cell.{ x, y, color }), row), matrix)
        |> fold_left(append, [||])
        |> to_list
        |> Board.sortCells
);

let (w, b, n) = (Some(Cell.White), Some(Cell.Black), None);

configureEnzyme(react_16_adapter());

describe("Board", () => {
    describe("initCells", () => {
        test("it should init a list of cells", () => {
            expect(Board.initCells(2, 2, None)) |> toEqual([
                Cell.({ x: 0, y: 0, color: None }),
                Cell.({ x: 1, y: 0, color: None }),
                Cell.({ x: 0, y: 1, color: None }),
                Cell.({ x: 1, y: 1, color: None }),
            ])
        });
    });

    describe("sortCells", () => {
        test("should sort cell by x/y coords", () => {
            expect(Board.sortCells([
                Cell.({ x: 1, y: 0, color: None }),
                Cell.({ x: 1, y: 3, color: None }),
                Cell.({ x: 2, y: 0, color: None }),
                Cell.({ x: 1, y: 2, color: None }),
                Cell.({ x: 1, y: 1, color: None }),
                Cell.({ x: 2, y: 2, color: None }),
            ])) |> toEqual([
                Cell.({ x: 1, y: 0, color: None }),
                Cell.({ x: 2, y: 0, color: None }),
                Cell.({ x: 1, y: 1, color: None }),
                Cell.({ x: 1, y: 2, color: None }),
                Cell.({ x: 2, y: 2, color: None }),
                Cell.({ x: 1, y: 3, color: None }),
            ])
        });
    });

    describe("getCell", () => {
        test("it should return cell color if exist", () => {
            expect(Board.getCell(1, 3, [
                Cell.({ x: 1, y: 0, color: None }),
                Cell.({ x: 2, y: 0, color: None }),
                Cell.({ x: 1, y: 1, color: None }),
                Cell.({ x: 1, y: 2, color: None }),
                Cell.({ x: 2, y: 2, color: None }),
                Cell.({ x: 1, y: 3, color: Some(Cell.White) }),
            ])) |> toEqual(Some(Cell.White));
        });

        test("it should return None if cell does not exist", () => {
            expect(Board.getCell(1, 4, [
                Cell.({ x: 1, y: 0, color: None }),
                Cell.({ x: 1, y: 2, color: None }),
                Cell.({ x: 2, y: 2, color: None }),
                Cell.({ x: 1, y: 3, color: Some(Cell.White) }),
            ])) |> toEqual(None);
        });
    });

    describe("init", () => {
        test("should return a new board", () => {
            expect(Board.init(4, 4)) |> toEqual(Board.({
                width: 4,
                height: 4,
                cells: matrixToCells([|
                    [|n, n, n, n|],
                    [|n, b, w, n|],
                    [|n, w, b, n|],
                    [|n, n, n, n|],
                |])
            }))
        });
    });

    describe("drawCells", () => {
        test("should return a new board with new cells", () => {
            expect(Board.drawCells(matrixToCells([|
                [|n, n|],
                [|n, n|],
            |]), [
                { x: 0, y: 0, color: Some(Cell.Black) },
                { x: 1, y: 1, color: Some(Cell.White) },
            ])) |> toEqual(matrixToCells([|
                [|b, n|],
                [|n, w|],
            |]))
        });
    });

    describe("getCountForColor", () => {
        test("should return the number of cells of corresponding color", () => Board.({
            expect(getCountForColor(init(4, 4), Some(Cell.Black))) |> toBe(2);
            expect(getCountForColor(init(4, 4), Some(Cell.White))) |> toBe(2);
            expect(getCountForColor(init(4, 4), None)) |> toBe(12);
        }));
    });

    describe("isOutOfBound", () => {
        let baseBoard = Board.init(4, 4);

        test("should return true for out of bounds coords", () => {
            expect(Board.isOutOfBound(baseBoard, (3, 4))) |> toBe(true);
            expect(Board.isOutOfBound(baseBoard, (4, 3))) |> toBe(true);
            expect(Board.isOutOfBound(baseBoard, (-1, 0))) |> toBe(true);
            expect(Board.isOutOfBound(baseBoard, (0, -1))) |> toBe(true);
        });

        test("should return false for inner bounds coords", () => {
            expect(Board.isOutOfBound(baseBoard, (0, 0))) |> toBe(false);
            expect(Board.isOutOfBound(baseBoard, (3, 3))) |> toBe(false);
            expect(Board.isOutOfBound(baseBoard, (1, 3))) |> toBe(false);
        });
    });

    describe("flippedCellsInDirection", () => {
        let baseBoard = Board.init(4, 4);

        test("should return empty list for an out of bounds cell change", () => {
            let cellChange = Cell.({ x: -1, y: 0, color: Some(Cell.Black) });

            expect(Board.flippedCellsInDirection(baseBoard, cellChange, (1, 1)))
                |> toBe([])
        });

        test("should return empty list for an empty cell in direction", () => {
            let cellChange = Cell.({ x: 0, y: 0, color: Some(Cell.Black) });

            expect(Board.flippedCellsInDirection(baseBoard, cellChange, (1, 0)))
                |> toBe([])
        });

        test("should return empty list for a same cell color next to cell change", () => {
            let cellChange = Cell.({ x: 1, y: 0, color: Some(Cell.Black) });
            
            expect(Board.flippedCellsInDirection(baseBoard, cellChange, (0, 1)))
                |> toBe([])
        });

        test("should return a filled list for a valid cell change", () => {
            let cellChange = Cell.({ x: 1, y: 0, color: Some(Cell.White) });
            
            expect(Board.flippedCellsInDirection(baseBoard, cellChange, (0, 1)))
                |> toEqual([
                    Cell.({ x: 1, y: 1, color: Some(Cell.White) }),
                ]) 
        });
    });

    describe("getFlippedCells", () => {
        let fakeBoard = Board.{
            width: 6,
            height: 6,
            cells: matrixToCells([|
                [|n, n, n, n, n, n|],
                [|n, b, n, n, n, n|],
                [|n, w, w, w, n, n|],
                [|n, n, w, b, n, n|],
                [|n, n, n, n, n, n|],
                [|n, n, n, n, n, n|]
            |])
        };

        test("should return flipped cells from cell change in all directions", () => {
            let cellChange = Cell.({ x: 1, y: 3, color: Some(Cell.Black) });
            
            expect(Board.getFlippedCells(fakeBoard, cellChange))
                |> toEqual([
                    Cell.({ x: 1, y: 3, color: Some(Cell.Black) }),
                    Cell.({ x: 1, y: 2, color: Some(Cell.Black) }),
                    Cell.({ x: 2, y: 3, color: Some(Cell.Black) }),
                ]) 
        });
    });

    describe("availableCellChanges", () => {
        test("should return all available cell changes for a color", () => {
            expect(Board.availableCellChanges(Board.init(4, 4), Some(Black)))
                |> toEqual([
                    Cell.({ x: 2, y: 0, color: Some(Black) }),
                    Cell.({ x: 3, y: 1, color: Some(Black) }),
                    Cell.({ x: 0, y: 2, color: Some(Black) }),
                    Cell.({ x: 1, y: 3, color: Some(Black) }),
                ])
        });
    });

    describe("applyCellChange", () => {
        let fakeBoard = Board.init(4, 4);

        test("should raise an InvalidMove if cell change target is not empty", () => Cell.({
            expect(fun () => Board.applyCellChange(fakeBoard, { x: 1, y: 1, color: Some(White) }))
                |> toThrowException(Board.InvalidMove)
        }));

        test("should raise an InvalidMove if cell change does not flip any cell", () => {
            expect(() => Board.applyCellChange(fakeBoard, Cell.{ x: 0, y: 0, color: Some(White) }))
                |> toThrowException(Board.InvalidMove)
        });

        test("should return an updated board if cell change is valid", () => Cell.({
            expect(Board.applyCellChange(fakeBoard, { x: 1, y: 0, color: Some(White) }))
                |> toEqual(Board.({
                    width: 4,
                    height: 4,
                    cells: matrixToCells([|
                        [|n, w, n, n|],
                        [|n, w, w, n|],
                        [|n, w, b, n|],
                        [|n, n, n, n|],
                    |]),
                }))
        }));
    });

    describe("<Board />", () => {
        test("should render a grid of <Cell />", () => {
            let board = Board.init(4, 4);
            let wrapper = shallow(<Board board onCellClick={(_) => ()} />);

            expect(wrapper |> find("Cell") |> length) |> toBe(16);
        });
    });
});

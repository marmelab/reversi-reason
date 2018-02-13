open Jest;
open Enzyme;
open Expect;

let [w, b, n] = [Some(Cell.White), Some(Cell.Black), None];

configureEnzyme(react_16_adapter());

describe("Board", () => {
    describe("init", () => {
        test("should return a new board", () => {
            expect(Board.init(4, 4)) |> toEqual(Board.({
                width: 4,
                height: 4,
                cells: [|
                    [|n, n, n, n|],
                    [|n, b, w, n|],
                    [|n, w, b, n|],
                    [|n, n, n, n|],
                |]
            }))
        });
    });

    describe("drawCells", () => {
        test("should return a new board with new cells", () => {
            expect(Board.drawCells([|
                [|n, n|],
                [|n, n|],
            |], [
                { x: 0, y: 0, color: Some(Cell.Black) },
                { x: 1, y: 1, color: Some(Cell.White) },
            ])) |> toEqual([|
                [|b, n|],
                [|n, w|],
            |])
        });
    });

    describe("getCountForColor", () => {
        test("should return the number of cells of corresponding color", () => Board.({
            expect(getCountForColor(init(4, 4), Some(Cell.Black))) |> toBe(2);
            expect(getCountForColor(init(4, 4), Some(Cell.White))) |> toBe(2);
            expect(getCountForColor(init(4, 4), None)) |> toBe(12);
        }));
    });

    describe("directions", () => {
        test("should be a valid representation of directions", () => {
            expect(Board.directions) |> toEqual([
                (0, 1),
                (0, -1),
                (1, 0),
                (1, 1),
                (1, -1),
                (-1, 0),
                (-1, 1),
                (-1, -1),
            ]);
        });
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
                    Cell.({ x: 1, y: 0, color: Some(Cell.White) }),
                    Cell.({ x: 1, y: 1, color: Some(Cell.White) }),
                ]) 
        });
    });

    describe("getFlippedCells", () => {
        let fakeBoard = Board.{
            width: 6,
            height: 6,
            cells: [|
                [|n, n, n, n, n, n|],
                [|n, b, n, n, n, n|],
                [|n, w, w, w, n, n|],
                [|n, n, w, b, n, n|],
                [|n, n, n, n, n, n|],
                [|n, n, n, n, n, n|]
            |]
        };

        test("should return flipped cells from cell change in all directions", () => {
            let cellChange = Cell.({ x: 1, y: 3, color: Some(Cell.Black) });
            
            expect(Board.getFlippedCells(fakeBoard, cellChange))
                |> toEqual([
                    Cell.({ x: 2, y: 3, color: Some(Cell.Black) }),
                    Cell.({ x: 1, y: 2, color: Some(Cell.Black) }),
                    Cell.({ x: 1, y: 3, color: Some(Cell.Black) }),
                ]) 
        });
    });

    describe("availableCellChanges", () => {
        test("should return all available cell changes for a color", () => Cell.({
            expect(Board.availableCellChanges(Board.init(4, 4), Some(Black)))
                |> toEqual([|
                    { x: 2, y: 0, color: Some(Black) },
                    { x: 3, y: 1, color: Some(Black) },
                    { x: 0, y: 2, color: Some(Black) },
                    { x: 1, y: 3, color: Some(Black) },
                |])
        }));
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
                    cells: [|
                        [|n, w, n, n|],
                        [|n, w, w, n|],
                        [|n, w, b, n|],
                        [|n, n, n, n|],
                    |],
                }))
        }));
    });

    describe("<Board />", () => {
        test("should render a grid of <Cell />", () => {
            let board = Board.init(4, 4);
            let wrapper = shallow(<Board board />);

            expect(wrapper |> find("Cell") |> length) |> toBe(16);
        });
    });
});

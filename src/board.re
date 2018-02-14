open Cell;
open ReasonReact;

type board = {
    width: int,
    height: int,
    cells: array(array(option(color)))
};

exception InvalidMove;

let init = (width, height) => {
    let cells = Array.make_matrix(width, height, None);
    let (midWidth, midHeight) = (width / 2, height / 2);

    cells[midWidth][midHeight] = Some(Black);
    cells[midWidth - 1][midHeight - 1] = Some(Black);
    cells[midWidth - 1][midHeight] = Some(White);
    cells[midWidth][midHeight - 1] = Some(White);

    { width, height, cells }
};

let drawCells = (cells, cellsToDraw) => {
    let newCells = Array.(map(copy, cells));
    
    List.iter(({ x, y, color }) => {
        newCells[y][x] = color;
    }, cellsToDraw);

    newCells
};

let getCountForColor = (board, color) => Array.(
    board.cells
        |> fold_left(append, [||])
        |> Js.Array.filter(c => c == color)
        |> length
);

let directions = List.(
    map(x => map(y => (x, y), [0, 1, -1]), [0, 1, -1])
    |> flatten
    |> filter(dir => dir != (0, 0)) 
);

let isOutOfBound = ({ width, height }, (x, y)) => 
    x < 0 || y < 0 || x > (width - 1) || y > (height - 1);

let flippedCellsInDirection = (board, cell, (dirx, diry)) => {
    let cells = ref([]);
    let cursor = ref((cell.x + dirx, cell.y + diry));
    let break = ref(false);

    while (! break^) {
        cells := switch(cursor^) {
        | cursor when isOutOfBound(board, cursor) => break := true; []
        | (x, y) => switch(board.cells[y][x]) {
            | None => break := true; []
            | color when (color == cell.color) => break := true; cells^
            | _ => {
                cursor := (x + dirx, y + diry);
                [{ x, y, color: cell.color }, ...cells^]
            }
        }
        };
    };

    List.length(cells^) > 0 ? [cell, ...cells^] : []
};

let getFlippedCells = (board, cell) => 
    directions
        |> List.map(flippedCellsInDirection(board, cell))
        |> List.flatten
        |> List.sort_uniq((c1, c2) => {
            /* cells are equals => pos = 0 (remove dup) */
            c1.x == c2.x && c1.y == c2.y ? 0 : 1
        });

let availableCellChanges = (board, color) => Array.(
    init(board.height, y => init(board.width, x => { x, y, color }))
        |> fold_left(append, [||])
        |> Js.Array.filter(cell => switch (getFlippedCells(board, cell)) {
            | [] => false
            | _ => true
        })
);

let applyCellChange = (board, cell) => {
    let cells = switch (board.cells[cell.y][cell.x]) {
        | None => {
            switch(getFlippedCells(board, cell)) {
                | [] => raise(InvalidMove)
                | flipped => drawCells(board.cells, flipped)
            }
        }
        | _ => raise(InvalidMove)
    };

    { ...board, cells }
};

let component = statelessComponent("Board");

let make = (~board, ~onCellClick=?, _children) => {
    let handleClick = (x, y) => switch(onCellClick) {
    | None => ()
    | Some(callback) => callback(x, y)
    };

    {
    ...component,
    render: (_self) => 
        <div className="board">
            (arrayToElement(Array.(
                mapi((y, row) => {
                    <div key=string_of_int(y) className="row">
                        (arrayToElement(
                            mapi((x, color) =>
                                <Cell
                                    color
                                    key=string_of_int(x)
                                    onClick={(_) => handleClick(x, y)}
                                />,
                            row)
                        ))
                    </div>
                }, board.cells)
            )))
        </div>
    }
};

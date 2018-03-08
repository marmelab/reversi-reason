open Cell;

type board = {
    width: int,
    height: int,
    cells: list(cell)
};

exception InvalidMove;

let sortCells = List.sort((c1, c2) => (c1.y * 10 + c1.x) - (c2.y * 10 + c2.x));

let initCells = (~width, ~height, ~color) => Belt.List.makeBy(
    width * height,
    i => ({
        x: i mod width,
        y: i / width,
        color,
    })
);

let drawCells = (cells, cellsToDraw) => cells
    |> List.filter(cell => !List.exists(
        ({ x, y }) => x == cell.x && y == cell.y,
        cellsToDraw
    ))
    |> List.append(cellsToDraw)
    |> sortCells;

let getCell = (x, y, cells) =>
    switch (List.find(cell => cell.x == x && cell.y == y, cells)) {
        | ({ color }) => color
        | exception Not_found => None
    };

let init = (width, height) => {
    let (mw, mh) = (width / 2, height / 2);

    let cells = drawCells(
        initCells(~width, ~height, ~color=None),
        [
            { x: mw, y: mh, color: Some(Black) },
            { x: mw - 1, y: mh - 1, color: Some(Black) },
            { x: mw - 1, y: mh, color: Some(White) },
            { x: mw, y: mh - 1, color: Some(White) }
        ],
    );

    { width, height, cells }
};

let getCountForColor = (board, color) =>
    board.cells
        |> List.filter(c => c.color == color)
        |> List.length;

let directions = [
    (0, 1),  /* S  */
    (0, -1), /* N  */
    (1, 0),  /* E  */
    (1, 1),  /* SE */
    (1, -1), /* NE */
    (-1, 0), /* O  */
    (-1, 1), /* SO */
    (-1, -1) /* NO */
];

let isOutOfBound = ({ width, height }, (x, y)) => 
    x < 0 || y < 0 || x > (width - 1) || y > (height - 1);

let flippedCellsInDirection = (board, cell, (dirx, diry)) => {
    let cells = ref([]);
    let cursor = ref((cell.x + dirx, cell.y + diry));
    let break = ref(false);

    while (! break^) {
        cells := switch(cursor^) {
            | cursor when isOutOfBound(board, cursor) => break := true; []
            | (x, y) => switch(getCell(x, y, board.cells)) {
                | None => break := true; []
                | color when (color == cell.color) => break := true; cells^
                | _ => {
                    cursor := (x + dirx, y + diry);
                    [{ x, y, color: cell.color }, ...cells^]
                }
            }
        };
    };

    cells^
};

let getFlippedCells = (board, cell) => {
    let flipped = directions
        |> List.map(flippedCellsInDirection(board, cell))
        |> List.flatten;

    switch (flipped) {
        | [] => []
        | cells => [cell, ...cells]
    };
};

let availableCellChanges = (board, color) => 
    initCells(~width=board.width, ~height=board.height, ~color)
        |> List.filter(cell => switch (getFlippedCells(board, cell)) {
            | [] => false
            | _ => true
        });

let applyCellChange = (board, cell) => {
    let cells = switch (getCell(cell.x, cell.y, board.cells)) {
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

let component = ReasonReact.statelessComponent("Board");

let make = (~board, ~onCellClick, _children) => {
    let cellWidth = string_of_float(100. /. float_of_int(board.width)) ++ "%";

    let cells = board.cells
        |> Array.of_list
        |> Array.map(cell => {
            <div
                key={string_of_int(cell.y) ++ string_of_int(cell.x)}
                style={ReactDOMRe.Style.make(~width=cellWidth, ())}
            >
                <Cell
                    color={cell.color}
                    onClick={(_) => onCellClick(cell)}
                />
            </div>
        });

    {
    ...component,
    render: (_self) => 
        <div className="board">
            (ReasonReact.arrayToElement(cells))
        </div>
    }
};

open Jest;
open Enzyme;
open Expect;

configureEnzyme(react_16_adapter());

describe("<App />", () => {
    test("should render a Game with game prop", () => {
        let game = Game.init([||]);
        let wrapper = shallow(<App game />);

        expect(wrapper |> find("Game") |> length) |> toBe(1);
    });
});
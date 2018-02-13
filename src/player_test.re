open Jest;
open Enzyme;
open Expect;

configureEnzyme(react_16_adapter());

describe("Player", () => {
    describe("bullet", () => {
        test("should return the right bullet icon for Black color", () => {
            expect(Player.bullet({ name: "foo", color: Cell.Black }))
                |> toBe({js|•|js})
        });

        test("should return the right bullet icon for White color", () => {
            expect(Player.bullet({ name: "foo", color: Cell.White }))
                |> toBe({js|○|js}) 
        });
    });

    describe("<Player />", () => {
        test("should render the player name", () => {
            let player = Player.{ name: "John", color: Cell.Black };
            let wrapper = shallow(<Player player current=false />);

            expect(wrapper |> find(".player") |> text) |> toEqual("John " ++ {js|•|js});
        });

        test("should render current as className if current", () => {
            let player = Player.{ name: "John", color: Cell.Black };
            let wrapper = shallow(<Player player current=true />);

            expect(wrapper |> find(".player.current") |> length) |> toBe(1);
        });

        test("should not render current as className if not current", () => {
            let player = Player.{ name: "John", color: Cell.Black };
            let wrapper = shallow(<Player player current=false />);

            expect(wrapper |> find(".player.current") |> length) |> toBe(0);
        });
    });
});
{
  description = "A C++ Webserver Nix Development Flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }: let
    system = "x86_64-linux";
  in {
    devShells."${system}".default = let
      pkgs = import nixpkgs {
        inherit system;
      };
    in pkgs.mkShell {
      packages = with pkgs; [
        gcc
        gnumake
        cmake
      ];

      shellHook = ''
        echo "C++ Webserver"
      '';
    };
  };
}

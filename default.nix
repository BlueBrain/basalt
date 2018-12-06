with import <BBPpkgs> { };
{
  pybasalt = python3Packages.basalt.overrideDerivation (oldAtr: rec {
      name = "basalt-DEV_ENV";
      src = ./.;
      makeFlags = [ "VERBOSE=1" ];
  });
}

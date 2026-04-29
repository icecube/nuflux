__all__ = [
    "FluxFunction",
    "LegacyConventionalFlux",
    "LegacyPromptFlux",
    "NuE",
    "NuEBar",
    "NuMu",
    "NuMuBar",
    "NuTau",
    "NuTauBar",
    "ParticleType",
    "SplineFlux",
    "SplineFlux2",
    "availableFluxes",
    "dataPath",
    "kneesForFlux",
    "makeFlux",
    "printModels",
]

from ._nuflux import (
    FluxFunction,
    LegacyConventionalFlux,
    LegacyPromptFlux,
    NuE,
    NuEBar,
    NuMu,
    NuMuBar,
    NuTau,
    NuTauBar,
    ParticleType,
    availableFluxes,
    dataPath,
    getVersion,
    kneesForFlux,
    makeFlux,
    printModels,
)

# Implementations which use splines may or may not be available
try:
    from ._nuflux import (
        SplineFlux,
        SplineFlux2,
    )
    __all__.extend([
        "SplineFlux",
        "SplineFlux2",
    ])
except ImportError:
    pass

__version__ = getVersion()

def __getattr__(name):
    if name == "stand_alone":
        # import inside a method is inefficient, but we're trying to discourage use of this anyway
        import warnings
        warnings.warn("The stand_alone property is deprecated", DeprecationWarning, stacklevel=2)
        return True

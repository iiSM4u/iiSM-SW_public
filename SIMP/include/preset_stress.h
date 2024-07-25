#pragma once

class preset_stress
{
public:
    preset_stress() = default;
    preset_stress(int index, int radius, int samples, int iterations, bool enhanceShadows)
        : index(index)
        , radius(radius)
        , samples(samples)
        , iterations(iterations)
        , enhanceShadows(enhanceShadows)
    {}

    int GetIndex() const { return this->index; }
    int GetRadius() const { return this->radius; }
    int GetSamples() const { return this->samples; }
    int GetIterations() const { return this->iterations; }
    bool GetEnhanceShadows() const { return this->enhanceShadows; }

private:
    int index, radius, samples, iterations;
    bool enhanceShadows;
};


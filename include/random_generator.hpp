class RandomGenerator{
public:
    explicit RandomGenerator(std::uint64_t seed);
    std::uint32_t uniform_unit(
        std::uint32_t minimum,
        std::uint32_t maximum
    );

private:
    std::mt19937_64 engine_;
}
#include <iostream>
#include <algorithm>
#include <array>
#include <iostream>
#include <regex>
#include <numeric>
#include <vector>
#include <deque>
#include <sstream>

// Вариант  10
// Составить  описание  класса  многочленов  от  одной  переменной, задаваемых  сте­пенью  многочлена
// и  массивом  коэффициентов.Предусмотреть  методы  для  вы­числения  значения  многочлена  для  заданного  аргумента,
// операции  сложения, вычитания  и умножения  многочленов  с получением  нового  объекта - многочлена, вывод  на  экран  описания  многочлена.
// Написать  программу, демонстрирующую  работу  с  этим  классом.

namespace math
{
	namespace polynom
	{
		template<typename DegreeT = uint32_t, typename CoefficientT = int>
		class Schema
		{
		public:
			std::vector<CoefficientT> coefficients;

			Schema(decltype(coefficients) coefficients)
				: coefficients(std::move(coefficients))
			{
			}

			DegreeT get_max_degree() const
			{
				return coefficients.size() - 1;
			}

			Schema operator+(Schema const &other)
			{
				auto coeff_count_in_both = (std::min)(coefficients.size(), other.coefficients.size());
				auto coeff_count = (std::max)(coefficients.size(), other.coefficients.size());
				decltype(coefficients) result_coefficients(coeff_count, 0);
				std::transform(
					coefficients.rbegin(),
					coefficients.rbegin() + coeff_count_in_both,
					other.coefficients.rbegin(),
					result_coefficients.rbegin(),
					std::plus<CoefficientT>()
				);

				if (coefficients.size() != other.coefficients.size())
				{
					auto& addictional_coefficients = coefficients.size() < other.coefficients.size()
						? other.coefficients
						: coefficients;

					std::copy(
						addictional_coefficients.rbegin() + coeff_count_in_both,
						addictional_coefficients.rend(),
						result_coefficients.rbegin() + coeff_count_in_both
					);
				}

				return Schema{ std::move(result_coefficients) };
			}
			Schema operator-(Schema const &other)
			{
				auto coeff_count_in_both = (std::min)(coefficients.size(), other.coefficients.size());
				auto coeff_count = (std::max)(coefficients.size(), other.coefficients.size());
				decltype(coefficients) result_coefficients(coeff_count, 0);
				std::transform(
					coefficients.rbegin(),
					coefficients.rbegin() + coeff_count_in_both,
					other.coefficients.rbegin(),
					result_coefficients.rbegin(),
					std::minus<CoefficientT>()
				);

				if (coefficients.size() != other.coefficients.size())
				{
					auto& addictional_coefficients = coefficients.size() < other.coefficients.size()
						? other.coefficients
						: coefficients;
					bool revert = coefficients.size() < other.coefficients.size();

					auto current = addictional_coefficients.rbegin() + coeff_count_in_both;
					auto end = addictional_coefficients.rend();
					auto result_current = result_coefficients.rbegin() + coeff_count_in_both;
					for (; current != end; ++current, ++result_current)
						*result_current = revert ? -*current : *current;
				}

				return Schema{ std::move(result_coefficients) };
			}
			Schema operator*(Schema const &other)
			{
				auto coeff_count = coefficients.size() + other.coefficients.size() - 1;
				decltype(coefficients) result_coefficients(coeff_count, 0);

				for (size_t i = 0; i < coefficients.size(); ++i)
					for (size_t j = 0; j < other.coefficients.size(); ++j)
						result_coefficients[i + j] += coefficients[i] * other.coefficients[j];

				return Schema{ std::move(result_coefficients) };
			}
		};
		template<typename ArgumentT = int, typename DegreeT = uint32_t, typename CoefficientT = int>
		class Instance
		{
		public:
			std::deque<CoefficientT> coefficients;
			using schema_type = Schema<DegreeT, CoefficientT>;
			using argument_type = ArgumentT;
			using coefficient_type = CoefficientT;

			std::shared_ptr<schema_type> schema;
			ArgumentT argument;

			Instance(decltype(schema) schema, decltype(argument) argument)
				: schema(schema)
				, argument(std::move(argument))
			{
			}
		};

		namespace calculator
		{
			template<typename Type>
			class ICalculator
			{
			public:
				using type = Type;
				using result_type = typename std::common_type<typename Type::coefficient_type, typename Type::argument_type>::type;

				virtual ~ICalculator() = default;
				[[noreturn]]
				virtual result_type calculate(Type const &polynom) const { throw std::logic_error{ "Calculator isn't implemented" }; }
			};

			template<typename Type>
			class Gorner
			{
			};
			template<typename DegreeT, typename CoefficientT, typename ArgumentT>
			class Gorner<Instance<ArgumentT, DegreeT, CoefficientT>>
				: public ICalculator<Instance<ArgumentT, DegreeT, CoefficientT>>
			{
			public:
				using result_type = typename std::common_type<CoefficientT, ArgumentT>::type;
				using type = Instance<ArgumentT, DegreeT, CoefficientT>;

				Gorner() = default;

				result_type calculate(type const &polynom) const override
				{
					using coefficient_cref_type = decltype(*polynom.coefficients.begin());

					return std::accumulate(
						polynom.schema->coefficients.begin(), polynom.schema->coefficients.end(),
						result_type{ 0 },
						[&polynom](result_type result, coefficient_cref_type coefficient)
					{
						return result = polynom.argument * result + coefficient;
					}
					);
				}
			};

			template<typename CalculatorT, typename ...Args>
			auto make_calculator(Args&& ...args)
				-> std::unique_ptr<ICalculator<typename CalculatorT::type>>
			{
				return std::make_unique<CalculatorT>(std::forward<Args>(args)...);
			}
		}

		namespace view
		{
			template<typename Type>
			class IView
			{
			public:
				using type = Type;
				virtual ~IView() = default;
				[[noreturn]]
				virtual std::string to_string(Type const &obj) const { throw std::logic_error{ "View isn't implemented" }; }
			};

			template<typename Type>
			class View
			{
			};
			template<typename DegreeT, typename CoefficientT>
			class View<Schema<DegreeT, CoefficientT>>
				: public IView<Schema<DegreeT, CoefficientT>>
			{
			public:
				using type = Schema<DegreeT, CoefficientT>;
				View() = default;

				std::string to_string(type const &polynom_schema) const override
				{
					std::stringstream stream;
					std::transform(
						polynom_schema.coefficients.begin(),
						polynom_schema.coefficients.end() - 1,
						std::ostream_iterator<std::string>(stream, " + "),
						[degree = polynom_schema.get_max_degree()](decltype(*polynom_schema.coefficients.begin()) coefficient) mutable
					{
						return std::to_string(coefficient) + "*x^" + std::to_string(degree--);
					}
					);
					stream << std::to_string(polynom_schema.coefficients.back()) + "*x^0";
					return stream.str();
				}
			};
			template<typename ArgumentT, typename DegreeT, typename CoefficientT>
			class View<Instance<ArgumentT, DegreeT, CoefficientT>>
				: public IView<Instance<ArgumentT, DegreeT, CoefficientT>>
			{
			public:
				using type = Instance<ArgumentT, DegreeT, CoefficientT>;
				View() = default;

				std::string to_string(type const &polynom_instance) const override
				{
					std::stringstream stream;
					std::transform(
						polynom_instance.schema->coefficients.begin(),
						polynom_instance.schema->coefficients.end() - 1,
						std::ostream_iterator<std::string>(stream, " + "),
						[degree = polynom_instance.schema->get_max_degree(), arg = polynom_instance.argument]
					(decltype(*polynom_instance.schema->coefficients.begin()) coefficient) mutable
					{
						return std::to_string(coefficient) + "*" + std::to_string(arg) + "^" + std::to_string(degree--);
					}
					);
					stream << std::to_string(polynom_instance.schema->coefficients.back()) + "*" + std::to_string(polynom_instance.argument) + "^0";
					return stream.str();
				}
			};

			template<typename ViewT, typename ...Args>
			auto make_view(Args&& ...args)
				-> std::unique_ptr<IView<typename ViewT::type>>
			{
				return std::make_unique<ViewT>(std::forward<Args>(args)...);
			}
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");

	std::shared_ptr<math::polynom::Schema<>> schema{ new math::polynom::Schema<>({ 3, 2, 4, 1 }) };
	math::polynom::Instance<> polynom{ schema, 1 };

	using namespace math::polynom::view;
	using namespace math::polynom::calculator;

	auto sv = make_view<View<decltype(schema)::element_type>>();
	auto pv = make_view<View<decltype(polynom)>>();
	auto calculator = make_calculator<Gorner<decltype(polynom)>>();

	std::cout << "Polynom schema: " << std::endl
		<< sv->to_string(*schema) << std::endl;
	std::cout << "Polynom instance: " << std::endl
		<< pv->to_string(polynom) << std::endl;
	std::cout << "Polynom instance calculation result: " << std::endl
		<< calculator->calculate(polynom) << std::endl;

	std::shared_ptr<math::polynom::Schema<>> other_schema{ new math::polynom::Schema<>({ 1, 3, 2 }) };
	math::polynom::Instance<> other_polynom{ schema, 5 };


	std::cout << std::endl;
	std::cout << "Second polynom schema: " << std::endl
		<< sv->to_string(*other_schema) << std::endl;
	std::cout << "Second polynom instance: " << std::endl
		<< pv->to_string(other_polynom) << std::endl;
	std::cout << "Second polynom instance calculation result: " << std::endl
		<< calculator->calculate(other_polynom) << std::endl;

	auto schema_sum_result = *schema + *other_schema;
	auto schema_diff_result = *other_schema - *schema;
	// auto schema_multiply_result = *other_schema * *schema;

	// std::cout << std::endl;
	// std::cout << "Two polynoms schemes summ: " << std::endl
		// << sv->to_string(schema_sum_result) << std::endl;
	// std::cout << "Two polynoms schemes difference(second minus first): " << std::endl
		// << sv->to_string(schema_diff_result) << std::endl;
	// std::cout << "Two polynoms schemes multiply: " << std::endl
		// << sv->to_string(schema_multiply_result) << std::endl;

	return 0;
}
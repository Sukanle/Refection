#include <type_traits>

#include "static/inhertance_traits.hpp"

using namespace reflect::Static;


// 基础测试类型定义
struct Base {};
struct BaseA {};
struct BaseB {};
struct BaseC {};

// ====================
// 1. 普通链式继承测试
// ====================
struct L1Derived : Base {};
struct L2Derived : L1Derived {};
struct L3Derived : L2Derived {};

static_assert(inheritance_traits_v<L3Derived, Base>);
static_assert(inheritance_traits_v<L2Derived, Base>);
static_assert(inheritance_traits_v<L1Derived, Base>);

// ==============================
// 2. 简单普通菱形继承测试
// ==============================
struct CommonBase {};
struct SimpleDiamondBase1 : CommonBase {};
struct SimpleDiamondBase2 : CommonBase {};
struct SimpleDiamondDerived : SimpleDiamondBase1, SimpleDiamondBase2 {};

// 应检测到重复继承CommonBase
using SimpleDiamondCheck =
    inheritance_traits<SimpleDiamondDerived,
                       type_list<SimpleDiamondBase1, SimpleDiamondBase2>,
                       CommonBase>;
static_assert(!SimpleDiamondCheck::value);
static_assert(std::is_same_v<SimpleDiamondCheck::conflict_base, CommonBase>);

// =============================
// 3. 简单虚菱形继承测试
// =============================
struct VirtualBase1 : virtual CommonBase {};
struct VirtualBase2 : virtual CommonBase {};
struct VirtualDiamondDerived : VirtualBase1, VirtualBase2 {};

// 虚继承应通过检查
using VirtualDiamondCheck =
    inheritance_traits<VirtualDiamondDerived,
                       type_list<VirtualBase1, VirtualBase2>, CommonBase>;
static_assert(VirtualDiamondCheck::value);

// ================================
// 4. 复杂普通菱形继承测试
// ================================
struct ComplexBase1 : BaseA {};
struct ComplexBase2 : BaseA, BaseB {};
struct ComplexBase3 : BaseB, BaseC {};
struct ComplexDiamondDerived : ComplexBase1, ComplexBase2, ComplexBase3 {};

// 检测BaseA的重复继承
using ComplexCheckA =
    inheritance_traits<ComplexDiamondDerived,
                       type_list<ComplexBase1, ComplexBase2, ComplexBase3>,
                       BaseB>;
static_assert(!ComplexCheckA::value);
static_assert(std::is_same_v<ComplexCheckA::conflict_base, BaseB>);

// 检测BaseB的重复继承
using ComplexCheckB =
    inheritance_traits<ComplexDiamondDerived,
                       type_list<ComplexBase2, ComplexBase3>, BaseB>;
static_assert(!ComplexCheckB::value);
static_assert(std::is_same_v<ComplexCheckB::conflict_base, BaseB>);

// 同时检测重复继承
using ComplexCheck =
    inheritance_traits<ComplexDiamondDerived,
                       type_list<ComplexBase2, ComplexBase3>, BaseB, BaseA>;
static_assert(!ComplexCheck::value);
static_assert(std::is_same_v<ComplexCheck::conflict_base, BaseB>);

// ===============================
// 5. 复杂虚菱形继承测试
// ===============================
struct VirtualComplexBase1 : virtual BaseA, BaseB {};
struct VirtualComplexBase2 : virtual BaseA, virtual BaseC {};
struct VirtualComplexDerived : VirtualComplexBase1, VirtualComplexBase2 {};

// 虚继承BaseA应通过检查
using VirtualComplexCheckA =
    inheritance_traits<VirtualComplexDerived,
                       type_list<VirtualComplexBase1, VirtualComplexBase2>,
                       BaseA>;
static_assert(VirtualComplexCheckA::value);

// 非共同继承BaseB会报错
using VirtualComplexCheckB =
    inheritance_traits<VirtualComplexDerived,
                       type_list<VirtualComplexBase1, VirtualComplexBase2>,
                       BaseB>;
static_assert(!VirtualComplexCheckB::value);
static_assert(std::is_same_v<VirtualComplexCheckB::conflict_base, BaseB>);

// ===============================
// 6. 混合继承测试
// ===============================
struct MixedBase1 : BaseA, BaseB {};
struct MixedBase2 : BaseA, virtual BaseB {};
struct MixedDerived : MixedBase1, MixedBase2 {};

// 检测BaseA的非虚冲突
using MixedCheck =
    inheritance_traits<MixedDerived, type_list<MixedBase1, MixedBase2>, BaseB,
                       BaseA>;
static_assert(!MixedCheck::value);
static_assert(std::is_same_v<MixedCheck::Validator::conflict_type, BaseA>);

// ===============================
// 7. 继承链遍历压力测试
// 构建一个较大的复杂继承图并验证 traversal 能找到所有基类且不重复
// ===============================
namespace inheritance_chain_tests {
using namespace reflect::Static;

// 构造多层多分支继承
struct N0 {};
struct N1a : N0 {};
struct N1b : N0 {};
struct N2a : N1a, N1b {};
struct N2b : N1a {};
struct N3 : N2a, virtual N2b {};

// 更广的候选集（包含一些不相关类型）
struct Unrelated {};
using Candidates = type_list<N3, N2a, N2b, N1a, N1b, N0, Unrelated>;

using Chain = inheritance_chain_t<N3, Candidates>;

static_assert(contains_v<Chain, N2a>);
static_assert(contains_v<Chain, N2b>);
static_assert(contains_v<Chain, N1a>);
static_assert(contains_v<Chain, N1b>);
static_assert(contains_v<Chain, N0>);
static_assert(!contains_v<Chain, Unrelated>);

// 递归深度压力：链式衍生类型
struct D0 {};
struct D1 : D0 {};
struct D2 : D1 {};
struct D3 : D2 {};
struct D4 : D3 {};
struct D5 : D4 {};
struct D6 : D5 {};
struct D7 : D6 {};
struct D8 : D7 {};
struct D9 : D8 {};

using DeepCandidates = type_list<D9, D8, D7, D6, D5, D4, D3, D2, D1, D0>;
using DeepChain = inheritance_chain_t<D9, DeepCandidates>;
static_assert(contains_v<DeepChain, D0>);
static_assert(contains_v<DeepChain, D5>);
static_assert(contains_v<DeepChain, D8>);

} // namespace inheritance_chain_tests

int main() {
    using namespace inheritance_chain_tests;
    reflect::Static::debug::print_chain_for<N3, Candidates>();
    reflect::Static::debug::print_tree_for<N3, Candidates>();
    return 0;
}


#include <type_traits>

#include "type_traits.hpp"

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

static_assert(Inheritance_traits_v<L3Derived, Base>);
static_assert(Inheritance_traits_v<L2Derived, Base>);
static_assert(Inheritance_traits_v<L1Derived, Base>);

// ==============================
// 2. 简单普通菱形继承测试
// ==============================
struct CommonBase {};
struct SimpleDiamondBase1 : CommonBase {};
struct SimpleDiamondBase2 : CommonBase {};
struct SimpleDiamondDerived : SimpleDiamondBase1, SimpleDiamondBase2 {};

// 应检测到重复继承CommonBase
using SimpleDiamondCheck =
    Inheritance_traits<SimpleDiamondDerived,
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
    Inheritance_traits<VirtualDiamondDerived,
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
    Inheritance_traits<ComplexDiamondDerived,
                       type_list<ComplexBase1, ComplexBase2, ComplexBase3>,
                       BaseB>;
static_assert(!ComplexCheckA::value);
static_assert(std::is_same_v<ComplexCheckA::conflict_base, BaseB>);

// 检测BaseB的重复继承
using ComplexCheckB =
    Inheritance_traits<ComplexDiamondDerived,
                       type_list<ComplexBase2, ComplexBase3>, BaseB>;
static_assert(!ComplexCheckB::value);
static_assert(std::is_same_v<ComplexCheckB::conflict_base, BaseB>);

// 同时检测重复继承
using ComplexCheck =
    Inheritance_traits<ComplexDiamondDerived,
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
    Inheritance_traits<VirtualComplexDerived,
                       type_list<VirtualComplexBase1, VirtualComplexBase2>,
                       BaseA>;
static_assert(VirtualComplexCheckA::value);

// 非共同继承BaseB会报错
using VirtualComplexCheckB =
    Inheritance_traits<VirtualComplexDerived,
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
    Inheritance_traits<MixedDerived, type_list<MixedBase1, MixedBase2>, BaseB,
                       BaseA>;
static_assert(!MixedCheck::value);
static_assert(std::is_same_v<MixedCheck::Validator::conflict_type, BaseA>);

int main() {}

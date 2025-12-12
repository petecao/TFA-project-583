#include "CallGraph.h"
#include <cstddef>
#include <utility>

bool CallGraphPass::getCPPVirtualFunc(Value* V, int &Idx, Type* &Sty){

    set<Value *>Visited;

	// REVISED
	if (GEPOperator *GEP = dyn_cast<GEPOperator>(V)) {
		Type *ElemTy = GEP->getResultElementType();

		if (!(ElemTy->isFunctionTy() || ElemTy->isPointerTy()))
			return false;

		if (!GEP->hasAllConstantIndices())
			return false;

		unsigned indice_num = GEP->getNumIndices();
		if (indice_num != 1)
			return false;

		// Recurse on the base pointer as before
		return getCPPVirtualFunc(GEP->getPointerOperand(), Idx, Sty);
	}

	// Case 2: LoadInst
	// Maybe we should also consider the store inst here
	else if (LoadInst *LI = dyn_cast<LoadInst>(V)) {
		return getCPPVirtualFunc(LI->getOperand(0), Idx, Sty);
	}

	else if (BitCastInst *BCI = dyn_cast<BitCastInst>(V)) {
		Value *Op = BCI->getOperand(0);
		Type *srctoTy = nullptr;

		if (auto *GEP = dyn_cast<GEPOperator>(Op)) {
			// If the vtable pointer comes from a GEP, the source element
			// type is the thing being indexed into (often a struct)
			Type *BaseTy = GEP->getSourceElementType();
			if (BaseTy && BaseTy->isStructTy())
				srctoTy = BaseTy;
		} else if (auto *AI = dyn_cast<AllocaInst>(Op)) {
			Type *AllocTy = AI->getAllocatedType();
			if (AllocTy && AllocTy->isStructTy())
				srctoTy = AllocTy;
		} else if (auto *GV = dyn_cast<GlobalVariable>(Op)) {
			Type *ValTy = GV->getValueType();
			if (ValTy && ValTy->isStructTy())
				srctoTy = ValTy;
		}

		if (srctoTy && srctoTy->isStructTy() && srctoTy->getNumContainedTypes() != 0) {
			// treat this as a valid C++ style vtable pattern when the source is a non-empty struct
			Sty = srctoTy;
			return Idx >= 0;
		}

		// Fall back to recursively analyzing the operand as before.
		return getCPPVirtualFunc(BCI->getOperand(0), Idx, Sty);
	}


#if 1
	// Other instructions such as CastInst
	// FIXME: may introduce false positives
	//UnaryInstruction includes castinst, load, etc, resolve this in the last step
	else if (UnaryInstruction *UI = dyn_cast<UnaryInstruction>(V)) {
		return getCPPVirtualFunc(UI->getOperand(0), Idx, Sty);
	}
#endif
	
	else {
		//OP<<"unknown inst: "<<*V<<"\n";
		return false;
	}

    return false;
}

bool endsWith(const string& str, const string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }

    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

//Resolve function types with variable parameters
//UPDATE: support func type casting checking in parameters
void CallGraphPass::resolveVariableParameters(CallInst *CI, FuncSet &FS, 
	bool enable_type_cast_in_args){

	Type* Ty = CI->getFunctionType();
	if(!Ty)
		return;

	string funcTy_str = getTypeStr(Ty);
	//OP<<"\nin variable parameter handling\n";
	//OP<<"CI: "<<*CI<<"\n";
	FunctionType* CI_funcTy = dyn_cast<FunctionType>(Ty);
	//OP<<"CI_funcTy: "<<*CI_funcTy<<"\n";
	auto CI_funcTy_arg_num = CI_funcTy->getNumParams();
	//OP<<"argnum: "<<CI_funcTy_arg_num<<"\n";

	//The function type contains variable parameters
	//Note: it seems that it also works for type casting without variable parameters
	//if(endsWith(funcTy_str, ", ...)")){
	
	if(endsWith(funcTy_str, ", ...)") || enable_type_cast_in_args){
		Type *CITy = CI->getFunctionType();


		//Erase ', ...)'
		funcTy_str.erase(funcTy_str.length() - 6);

		for(auto i = Ctx->sigFuncsMap.begin(); i != Ctx->sigFuncsMap.end(); i++){

			size_t type_hash = i->first;
			auto func_set = i->second;

			if(func_set.empty())
				continue;

			Function* f = *func_set.begin();
			Type* fty = f->getFunctionType();
			if(!fty)
				continue;

			FunctionType* f_funcTy = dyn_cast<FunctionType>(fty);

			//Check whether fty could become funcTy through casting
			auto f_funcTy_arg_num = f_funcTy->getNumParams();
			if(f_funcTy_arg_num < CI_funcTy_arg_num)
				continue;

			vector<Type*> CI_funcTy_check_list;
			vector<Type*> f_funcTy_check_list;
			CI_funcTy_check_list.push_back(CI_funcTy->getReturnType());
			for(int i = 0; i < CI_funcTy_arg_num; i++){
				CI_funcTy_check_list.push_back(CI_funcTy->getParamType(i));
			}
			f_funcTy_check_list.push_back(f_funcTy->getReturnType());
			for(int i = 0; i < f_funcTy_arg_num; i++){
				f_funcTy_check_list.push_back(f_funcTy->getParamType(i));
			}

			bool is_valid_type = true;
			for(int i = 0; i < CI_funcTy_check_list.size(); i++){
				Type* CI_check_ty = CI_funcTy_check_list[i];
				Type* f_check_ty = f_funcTy_check_list[i];
				
				if(CI_check_ty == f_check_ty)
					continue;
				
				if(CI_check_ty->getTypeID() != f_check_ty->getTypeID()){
					is_valid_type = false;
					break;
				}
				
				if(CI_check_ty->isStructTy() && f_check_ty->isStructTy()){
					StructType* CI_check_Sty = dyn_cast<StructType>(CI_check_ty);
					StructType* f_check_Sty = dyn_cast<StructType>(f_check_ty);
					string CI_check_Sty_name = "";
					string f_check_Sty_name = "";

					if(CI_check_Sty->isLiteral()){
						CI_check_Sty_name = Ctx->Global_Literal_Struct_Map[typeHash(CI_check_Sty)];
					}
					else{
						StringRef Ty_name = CI_check_Sty->getStructName();
						CI_check_Sty_name = parseIdentifiedStructName(Ty_name);
					}

					if(f_check_Sty->isLiteral()){
						f_check_Sty_name = Ctx->Global_Literal_Struct_Map[typeHash(f_check_Sty)];
					}
					else{
						StringRef Ty_name = f_check_Sty->getStructName();
						f_check_Sty_name = parseIdentifiedStructName(Ty_name);
					}

					if(CI_check_Sty_name == f_check_Sty_name)
						continue;

					list<size_t> LT;
					set<size_t> PT;
					LT.push_back(typeHash(CI_check_Sty));
					bool found_cast = false;

					while (!LT.empty()) {
						size_t CT = LT.front();
						LT.pop_front();

						if (PT.find(CT) != PT.end())
							continue;
						PT.insert(CT);

						for (auto H : typeTransitMap[CT]){
							Type* Hty = hashTypeMap[H];

							if(Hty == f_check_ty){
								found_cast = true;
								break;
							}

							if(Hty->isStructTy()){
								StructType* SHty = dyn_cast<StructType>(Hty);
								string SHty_name = "";
								if(SHty->isLiteral()){
									SHty_name = Ctx->Global_Literal_Struct_Map[typeHash(Hty)];
								}
								else{
									StringRef Ty_name = Hty->getStructName();
									SHty_name = parseIdentifiedStructName(Ty_name);
								}
								
								if(SHty_name == f_check_Sty_name){
									found_cast = true;
									break;
								}
							}

							LT.push_back(H);
						}
						if(found_cast){
							break;
						}
					}

					if(found_cast == true)
						continue;

				}

				is_valid_type = false;
				break;
			}

			if(is_valid_type){
				for(auto ele : func_set){
					Ctx->sigFuncsMap[callHash(CI)].insert(ele);
				}
			}
			else{
				//OP<<"not found\n";
			}
		}
	}
	FS = Ctx->sigFuncsMap[callHash(CI)];
}

void CallGraphPass::getTargetsInDerivedClass(string class_name, int Idx, 
	FuncSet &FS){

	if(class_name == "")
		return;

	if(Ctx->Global_Class_Method_Index_Map.count(class_name) == 0)
		return;

	set<string> method_set;
	
	//First collect all virtual calls of current class
	for(string method_name : Ctx->Global_Class_Method_Index_Map[class_name][Idx]){
		method_set.insert(method_name);
	}

	//Then collect virtual calls of derived class
	list<string> worklist;
	set<string> analyzedlist;
	worklist.push_back(class_name);

	while (!worklist.empty()) {

		string current_class_name = worklist.front();
		worklist.pop_front();
		if (analyzedlist.find(current_class_name) != analyzedlist.end()){
			continue;
		}
		analyzedlist.insert(current_class_name);

		if(Ctx->Global_Class_Method_Index_Map.count(current_class_name) == 0)
			continue;

		if(Ctx->Global_VTable_Map.count(current_class_name) == 0)
			continue;

		for(string method_name : method_set){

			if(Ctx->Global_VTable_Map[current_class_name].count(method_name) == 0)
				continue;

			for(auto vf : Ctx->Global_VTable_Map[current_class_name][method_name]){
				if (Function *F = dyn_cast<Function>(vf)) {
					FS.insert(F);
				}
			}
		}

		if(Ctx->GlobalClassHierarchyMap.count(current_class_name)){
			for(string derived_class_name : Ctx->GlobalClassHierarchyMap[current_class_name])
				worklist.push_back(derived_class_name);
		}
	}
}
; ModuleID = 'mehdi_tests/tbaa_conflict_test.bc'
source_filename = "mehdi_tests/tbaa_conflict_test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.DoubleSlot = type { double, ptr }

@gi = dso_local local_unnamed_addr global i32 0, align 4, !dbg !0
@gd = dso_local local_unnamed_addr global double 0.000000e+00, align 8, !dbg !23
@islot = dso_local local_unnamed_addr global { i32, [4 x i8], ptr } { i32 0, [4 x i8] zeroinitializer, ptr @cb_int }, align 8, !dbg !5
@dslot = dso_local local_unnamed_addr global %struct.DoubleSlot { double 0.000000e+00, ptr @cb_double }, align 8, !dbg !16

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @cb_int() #0 !dbg !33 {
  store i32 1, ptr @gi, align 4, !dbg !34, !tbaa !35
  ret void, !dbg !39
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @cb_double() #0 !dbg !40 {
  store double 2.000000e+00, ptr @gd, align 8, !dbg !41, !tbaa !42
  ret void, !dbg !44
}

; Function Attrs: nounwind uwtable
define dso_local void @drive(i32 noundef %0) local_unnamed_addr #1 !dbg !45 {
    #dbg_value(i32 %0, !49, !DIExpression(), !51)
  %2 = icmp eq i32 %0, 0, !dbg !52
    #dbg_value(ptr poison, !50, !DIExpression(), !51)
    #dbg_value(ptr poison, !50, !DIExpression(), !51)
  br i1 %2, label %4, label %3, !dbg !52

3:                                                ; preds = %1
  store i32 10, ptr @islot, align 8, !dbg !54, !tbaa !56
  br label %5, !dbg !59

4:                                                ; preds = %1
  store double 2.000000e+01, ptr @dslot, align 8, !dbg !60, !tbaa !62
  br label %5

5:                                                ; preds = %4, %3
  %6 = phi ptr [ getelementptr inbounds nuw (i8, ptr @islot, i64 8), %3 ], [ getelementptr inbounds nuw (i8, ptr @dslot, i64 8), %4 ]
  %7 = load ptr, ptr %6, align 8, !dbg !64, !tbaa !65
    #dbg_value(ptr %7, !50, !DIExpression(), !51)
  tail call void %7() #2, !dbg !66
  ret void, !dbg !67
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!25, !26, !27, !28, !29, !30, !31}
!llvm.ident = !{!32}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "gi", scope: !2, file: !3, line: 5, type: !10, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C11, file: !3, producer: "clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "mehdi_tests/tbaa_conflict_test.c", directory: "/n/eecs583b/home/mrzaidi/research_project/TFA-project-583", checksumkind: CSK_MD5, checksum: "ada40d1ff05c38478137b9125cf2807d")
!4 = !{!5, !16, !0, !23}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(name: "islot", scope: !2, file: !3, line: 28, type: !7, isLocal: false, isDefinition: true)
!7 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "IntSlot", file: !3, line: 18, size: 128, elements: !8)
!8 = !{!9, !11}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "pad", scope: !7, file: !3, line: 19, baseType: !10, size: 32)
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "fp", scope: !7, file: !3, line: 20, baseType: !12, size: 64, offset: 64)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "fp_t", file: !3, line: 3, baseType: !13)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DISubroutineType(types: !15)
!15 = !{null}
!16 = !DIGlobalVariableExpression(var: !17, expr: !DIExpression())
!17 = distinct !DIGlobalVariable(name: "dslot", scope: !2, file: !3, line: 29, type: !18, isLocal: false, isDefinition: true)
!18 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "DoubleSlot", file: !3, line: 23, size: 128, elements: !19)
!19 = !{!20, !22}
!20 = !DIDerivedType(tag: DW_TAG_member, name: "pad", scope: !18, file: !3, line: 24, baseType: !21, size: 64)
!21 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "fp", scope: !18, file: !3, line: 25, baseType: !12, size: 64, offset: 64)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(name: "gd", scope: !2, file: !3, line: 6, type: !21, isLocal: false, isDefinition: true)
!25 = !{i32 7, !"Dwarf Version", i32 5}
!26 = !{i32 2, !"Debug Info Version", i32 3}
!27 = !{i32 1, !"wchar_size", i32 4}
!28 = !{i32 8, !"PIC Level", i32 2}
!29 = !{i32 7, !"PIE Level", i32 2}
!30 = !{i32 7, !"uwtable", i32 2}
!31 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!32 = !{!"clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)"}
!33 = distinct !DISubprogram(name: "cb_int", scope: !3, file: !3, line: 9, type: !14, scopeLine: 9, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2)
!34 = !DILocation(line: 10, column: 8, scope: !33)
!35 = !{!36, !36, i64 0}
!36 = !{!"int", !37, i64 0}
!37 = !{!"omnipotent char", !38, i64 0}
!38 = !{!"Simple C/C++ TBAA"}
!39 = !DILocation(line: 11, column: 1, scope: !33)
!40 = distinct !DISubprogram(name: "cb_double", scope: !3, file: !3, line: 13, type: !14, scopeLine: 13, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2)
!41 = !DILocation(line: 14, column: 8, scope: !40)
!42 = !{!43, !43, i64 0}
!43 = !{!"double", !37, i64 0}
!44 = !DILocation(line: 15, column: 1, scope: !40)
!45 = distinct !DISubprogram(name: "drive", scope: !3, file: !3, line: 31, type: !46, scopeLine: 31, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !48)
!46 = !DISubroutineType(types: !47)
!47 = !{null, !10}
!48 = !{!49, !50}
!49 = !DILocalVariable(name: "cond", arg: 1, scope: !45, file: !3, line: 31, type: !10)
!50 = !DILocalVariable(name: "fp", scope: !45, file: !3, line: 32, type: !12)
!51 = !DILocation(line: 0, scope: !45)
!52 = !DILocation(line: 34, column: 9, scope: !53)
!53 = distinct !DILexicalBlock(scope: !45, file: !3, line: 34, column: 9)
!54 = !DILocation(line: 36, column: 19, scope: !55)
!55 = distinct !DILexicalBlock(scope: !53, file: !3, line: 34, column: 15)
!56 = !{!57, !36, i64 0}
!57 = !{!"IntSlot", !36, i64 0, !58, i64 8}
!58 = !{!"any pointer", !37, i64 0}
!59 = !DILocation(line: 38, column: 5, scope: !55)
!60 = !DILocation(line: 40, column: 19, scope: !61)
!61 = distinct !DILexicalBlock(scope: !53, file: !3, line: 38, column: 12)
!62 = !{!63, !43, i64 0}
!63 = !{!"DoubleSlot", !43, i64 0, !58, i64 8}
!64 = !DILocation(line: 0, scope: !53)
!65 = !{!58, !58, i64 0}
!66 = !DILocation(line: 44, column: 5, scope: !45)
!67 = !DILocation(line: 45, column: 1, scope: !45)

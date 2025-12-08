; ModuleID = 'tests/test2.bc'
source_filename = "tests/test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.S = type { i32, double }

@gs = dso_local local_unnamed_addr global %struct.S zeroinitializer, align 8, !dbg !0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) uwtable
define dso_local void @use(ptr nocapture noundef writeonly initializes((0, 4), (8, 16)) %0, i32 noundef %1) local_unnamed_addr #0 !dbg !20 {
    #dbg_value(ptr %0, !25, !DIExpression(), !33)
    #dbg_value(i32 %1, !26, !DIExpression(), !33)
    #dbg_value(ptr %0, !27, !DIExpression(), !33)
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8, !dbg !34
    #dbg_value(ptr %3, !29, !DIExpression(), !33)
    #dbg_value(i32 %1, !31, !DIExpression(), !33)
  %4 = sitofp i32 %1 to double, !dbg !35
    #dbg_value(double %4, !32, !DIExpression(), !33)
  store i32 %1, ptr %0, align 4, !dbg !36, !tbaa !37
  store double %4, ptr %3, align 8, !dbg !41, !tbaa !42
  ret void, !dbg !44
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!12, !13, !14, !15, !16, !17, !18}
!llvm.ident = !{!19}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "gs", scope: !2, file: !3, line: 6, type: !7, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C11, file: !3, producer: "clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !4, globals: !6, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "tests/test2.c", directory: "/n/eecs583b/home/mrzaidi/research_project/TFA-project-583", checksumkind: CSK_MD5, checksum: "ed5cd2327f19fddc6c6ad5222b3e9368")
!4 = !{!5}
!5 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!6 = !{!0}
!7 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "S", file: !3, line: 1, size: 128, elements: !8)
!8 = !{!9, !11}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !7, file: !3, line: 2, baseType: !10, size: 32)
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !7, file: !3, line: 3, baseType: !5, size: 64, offset: 64)
!12 = !{i32 7, !"Dwarf Version", i32 5}
!13 = !{i32 2, !"Debug Info Version", i32 3}
!14 = !{i32 1, !"wchar_size", i32 4}
!15 = !{i32 8, !"PIC Level", i32 2}
!16 = !{i32 7, !"PIE Level", i32 2}
!17 = !{i32 7, !"uwtable", i32 2}
!18 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!19 = !{!"clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)"}
!20 = distinct !DISubprogram(name: "use", scope: !3, file: !3, line: 8, type: !21, scopeLine: 8, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !24)
!21 = !DISubroutineType(types: !22)
!22 = !{null, !23, !10}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!24 = !{!25, !26, !27, !29, !31, !32}
!25 = !DILocalVariable(name: "p", arg: 1, scope: !20, file: !3, line: 8, type: !23)
!26 = !DILocalVariable(name: "x", arg: 2, scope: !20, file: !3, line: 8, type: !10)
!27 = !DILocalVariable(name: "pa", scope: !20, file: !3, line: 9, type: !28)
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64)
!29 = !DILocalVariable(name: "pb", scope: !20, file: !3, line: 10, type: !30)
!30 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!31 = !DILocalVariable(name: "va", scope: !20, file: !3, line: 12, type: !10)
!32 = !DILocalVariable(name: "vb", scope: !20, file: !3, line: 13, type: !5)
!33 = !DILocation(line: 0, scope: !20)
!34 = !DILocation(line: 10, column: 22, scope: !20)
!35 = !DILocation(line: 13, column: 17, scope: !20)
!36 = !DILocation(line: 15, column: 9, scope: !20)
!37 = !{!38, !38, i64 0}
!38 = !{!"int", !39, i64 0}
!39 = !{!"omnipotent char", !40, i64 0}
!40 = !{!"Simple C/C++ TBAA"}
!41 = !DILocation(line: 16, column: 9, scope: !20)
!42 = !{!43, !43, i64 0}
!43 = !{!"double", !39, i64 0}
!44 = !DILocation(line: 17, column: 1, scope: !20)

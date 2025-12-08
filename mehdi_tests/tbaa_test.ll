; ModuleID = 'tbaa_test.bc'
source_filename = "tbaa_test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local void @foo() local_unnamed_addr #0 !dbg !10 {
  ret void, !dbg !13
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local void @bar() local_unnamed_addr #0 !dbg !14 {
  ret void, !dbg !15
}

; Function Attrs: nounwind uwtable
define dso_local void @test(i32 noundef %0) local_unnamed_addr #1 !dbg !16 {
    #dbg_value(i32 %0, !21, !DIExpression(), !26)
  %2 = icmp sgt i32 %0, 0, !dbg !27
  %3 = select i1 %2, ptr @foo, ptr @bar
    #dbg_value(ptr %3, !22, !DIExpression(), !26)
  tail call void (...) %3() #2, !dbg !29, !callees !30
  ret void, !dbg !31
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "tbaa_test.c", directory: "/n/eecs583b/home/mrzaidi/research_project/TFA-project-583", checksumkind: CSK_MD5, checksum: "bba7782eb7eccbaeb5e67507a467a5af")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!9 = !{!"clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)"}
!10 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 1, type: !11, scopeLine: 1, flags: DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0)
!11 = !DISubroutineType(types: !12)
!12 = !{null}
!13 = !DILocation(line: 1, column: 13, scope: !10)
!14 = distinct !DISubprogram(name: "bar", scope: !1, file: !1, line: 2, type: !11, scopeLine: 2, flags: DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0)
!15 = !DILocation(line: 2, column: 13, scope: !14)
!16 = distinct !DISubprogram(name: "test", scope: !1, file: !1, line: 4, type: !17, scopeLine: 4, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !20)
!17 = !DISubroutineType(types: !18)
!18 = !{null, !19}
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !{!21, !22}
!21 = !DILocalVariable(name: "x", arg: 1, scope: !16, file: !1, line: 4, type: !19)
!22 = !DILocalVariable(name: "fp", scope: !16, file: !1, line: 5, type: !23)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DISubroutineType(types: !25)
!25 = !{null, null}
!26 = !DILocation(line: 0, scope: !16)
!27 = !DILocation(line: 7, column: 11, scope: !28)
!28 = distinct !DILexicalBlock(scope: !16, file: !1, line: 7, column: 9)
!29 = !DILocation(line: 12, column: 5, scope: !16)
!30 = !{ptr @bar, ptr @foo}
!31 = !DILocation(line: 13, column: 1, scope: !16)

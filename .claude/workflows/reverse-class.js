export const meta = {
  name: 'reverse-class',
  description: 'Reverse the remaining functions of one GTA:SA class in parallel: dossier -> reverse+validate per function (isolated worktrees) -> adversarial review -> report',
  whenToUse: 'Batch-reverse a class (args: {cls: "CGarage", max: 8, maxBytes: 1500}). Each function ends up on its own git branch for you to review and merge.',
  phases: [
    { title: 'Plan', detail: 'class_dossier.py -> ordered list of functions to reverse' },
    { title: 'Reverse', detail: 'one gta-reverser per function, in its own worktree, validated with asm_validate + build' },
    { title: 'Review', detail: 'gta-reversal-reviewer checks each diff against the original (IDA / asm)' },
    { title: 'Report', detail: 'summary of branches, validation and verdicts' },
  ],
}

const cls = (args && args.cls) || null
if (!cls) throw new Error('pass args: {cls: "ClassName", max?: number, maxBytes?: number}')
const MAX = (args && args.max) || 8
const MAX_BYTES = (args && args.maxBytes) || 1500

const PLAN_SCHEMA = {
  type: 'object',
  properties: {
    functions: {
      type: 'array',
      items: {
        type: 'object',
        properties: {
          name: { type: 'string' },
          address: { type: 'string' },
          size: { type: 'number' },
          status: { type: 'string' },
          reason: { type: 'string' },
        },
        required: ['name', 'address', 'size', 'status'],
      },
    },
    skipped: { type: 'array', items: { type: 'string' } },
  },
  required: ['functions', 'skipped'],
}

const RESULT_SCHEMA = {
  type: 'object',
  properties: {
    function: { type: 'string' },
    outcome: { type: 'string', enum: ['reversed', 'partial', 'blocked'] },
    branch: { type: 'string' },
    diff: { type: 'string' },
    asmValidate: { type: 'string' },
    build: { type: 'string', enum: ['passed', 'failed', 'not-run'] },
    notes: { type: 'string' },
  },
  required: ['function', 'outcome', 'branch', 'diff', 'asmValidate', 'build', 'notes'],
}

const VERDICT_SCHEMA = {
  type: 'object',
  properties: {
    verdict: { type: 'string', enum: ['approve', 'approve-with-nits', 'changes-required'] },
    blocking: { type: 'array', items: { type: 'string' } },
    nits: { type: 'array', items: { type: 'string' } },
  },
  required: ['verdict', 'blocking', 'nits'],
}

phase('Plan')
const plan = await agent(
  `Plan the reversal of the GTA:SA class \`${cls}\` in this repo (gta-reversed).
Run: python .claude/skills/find-reversing-targets/scripts/class_dossier.py ${cls}
From its "Suggested order" pick up to ${MAX} NOT-reversed methods (status STUB, NOHOOK, KNOWN or MISSING) that are
<= ${MAX_BYTES} bytes, preferring "all callees available". Skip constructors/destructors of polymorphic classes,
__usercall functions and anything the dossier marks as blocked; list what you skipped and why in \`skipped\`.
Return the functions in the suggested order. Addresses must come from the dossier output (never invent).`,
  { label: `plan:${cls}`, phase: 'Plan', schema: PLAN_SCHEMA }
)
if (!plan || !plan.functions.length) {
  log(`Nothing to reverse for ${cls}` + (plan ? ` (skipped: ${plan.skipped.join('; ')})` : ''))
  return { cls, results: [], skipped: plan ? plan.skipped : [] }
}
log(`${plan.functions.length} function(s) planned for ${cls}; skipped ${plan.skipped.length}`)

const results = await pipeline(
  plan.functions,
  (fn) => agent(
    `Reverse \`${fn.name}\` (GTA address ${fn.address}, ${fn.size} bytes, repo status ${fn.status}) of class \`${cls}\`.
Follow the reverse-function skill end to end (IDA MCP if available, otherwise asm_validate.py --dump), including:
- register/flip the hook correctly (remove { .Reversed = false } only when fully reversed);
- python .claude/skills/validate-against-binary/scripts/asm_validate.py ${fn.address}  -> no [MISSING] (explain every [CHECK]);
- python .claude/skills/review-reversal/scripts/check_hooks.py <changed files> -> no [E];
- cmake --build build --config Debug --target gta_reversed (report failures honestly; if the build dir is missing say so);
- if the function is testable in isolation, add a NOTSA_DIFFTEST in source/difftests/tests/.
Only touch what this function needs. When done, commit on a new branch named reverse/${cls}-${fn.name.replace(/[^A-Za-z0-9_]/g, '_')}
(do NOT push), and return the branch name, the full \`git diff HEAD~1\`, the asm_validate summary and the build result.
If you cannot reverse it faithfully, return outcome "blocked" with the reason instead of guessing.`,
    { label: `reverse:${fn.name}`, phase: 'Reverse', agentType: 'gta-reverser', isolation: 'worktree', schema: RESULT_SCHEMA }
  ),
  (res, fn) => res && res.outcome !== 'blocked'
    ? agent(
      `Review this reversal of \`${fn.name}\` (GTA ${fn.address}) of class \`${cls}\`. Be adversarial: try to find any
behavioural difference from the original. Decompile/disassemble the original (IDA MCP, or
python .claude/skills/validate-against-binary/scripts/asm_validate.py ${fn.address} --dump) and compare line by line:
conditions (x87 compares), constants (bit-exact), call order, integer widths, RNG calls, hook registration.
asm_validate result reported by the author:
${res.asmValidate}
Build: ${res.build}. Author notes: ${res.notes}
DIFF:
${res.diff}`,
      { label: `review:${fn.name}`, phase: 'Review', agentType: 'gta-reversal-reviewer', schema: VERDICT_SCHEMA }
    ).then(v => ({ ...res, review: v }))
    : Promise.resolve(res ? { ...res, review: null } : { function: fn.name, outcome: 'blocked', notes: 'agent failed' })
)

phase('Report')
const done = results.filter(Boolean)
const approved = done.filter(r => r.review && r.review.verdict !== 'changes-required')
log(`${approved.length}/${done.length} approved; blocked: ${done.filter(r => r.outcome === 'blocked').length}`)
return {
  cls,
  skipped: plan.skipped,
  results: done.map(r => ({
    function: r.function,
    outcome: r.outcome,
    branch: r.branch || null,
    build: r.build || null,
    verdict: r.review ? r.review.verdict : null,
    blocking: r.review ? r.review.blocking : [],
    nits: r.review ? r.review.nits : [],
    asmValidate: r.asmValidate || null,
    notes: r.notes,
  })),
}

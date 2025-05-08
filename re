


---

Guiding Principles

Rules use input. and output. MVEL-style DSL for readability.

Rules are stored and processed as SQL, not in Java heap.

The inference engine evaluates rule logic dynamically by compiling DSL into SQL queries.

The system is metadata-driven, supports dynamic rule updates, and exposes REST APIs.



---

Final Design Architecture

1. Rule Metadata Model

Tables

rules

id	name	type	input_fields	computation (DSL)	output_fields	priority	status

1	HighLoanRule	eligibility	input.loan.amount	input.loan.amount > 100000	output.eligible=true	1	active


field_mappings

dsl_key	table_name	column_name	json_path (optional)

input.loan.amount	loans	amount	NULL
input.loan.utilization	loans	utilization	NULL


program_rules

program_id	rule_id	usage_type (eligibility/replenishment/priority)

101	1	eligibility



---

2. Rule DSL Parser & Translator

Converts MVEL-style DSL into SQL using field_mappings

DSL: input.loan.amount > 100000 AND input.loan.utilization < 0.7

Resolved SQL:

SELECT * FROM loans WHERE amount > 100000 AND utilization < 0.7;


> The parser uses regex to find input. keys and replaces them using the mapping table.




---

3. Inference Engine (SQL Executor)

Core Responsibilities:

Accept rule ID(s) or program ID.

Use the parser to compile rule into SQL.

Query the database and return:

Boolean for single-object evaluation.

List of loan IDs for bulk evaluation.


Can be extended with templates for insert/update actions too.


> This engine is stateless and can scale horizontally.




---

4. Rule Service API Layer

Endpoints:

POST /evaluate/rule/{ruleId} → returns Boolean or list

POST /evaluate/program/{programId} → batch evaluation using all rules

POST /rules/upload → dynamic rule creation/updating

GET /rules/catalog → metadata for UI/configuration



---

5. Audit & Logging

rule_audit_logs

id	rule_id	object_id	result	timestamp	executed_sql



Logs every evaluation for traceability and debugging.



---

Do We Use MVEL?

Not as an engine.
You use MVEL syntax (e.g., input.loan.amount > 100000) to describe rules but do not execute them via MVEL. Instead, you:

Store DSL-style expressions in DB

Parse and compile them into SQL dynamically

Run SQL against your database — this is your core advantage (speed, scalability, no heap footprint).



---

Benefits of This Design

Generic: Not tied to loan domain — works with any tabular data.

Extensible: Add new rules, rule types, and domains with zero code changes.

Performant: SQL execution, not object-based rule evaluation.

Maintainable: Rules and mappings in DB, manageable by rule writers or admin UI.

Auditable: Every action logged for review or debugging.



---

Would you like me to generate a diagram of this architecture next?


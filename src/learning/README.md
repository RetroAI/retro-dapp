# retro.ai learning environment

## Dependencies

This project is managed by poetry. Install it with:

```bash
pip install poetry
```

Then, install the dependencies with:

```bash
poetry install
```

## Running the tests

To run the tests, use the following command:

```bash
poetry run ci
```

## Running the formatters

To run the formatters, use the following command:

```bash
poetry run format
```

## Running the agents

Learning agents are placed in the `agents` directory. To run an agent, invoke it directly:

```bash
cd agents
./random_agent.py
```

module App;

import :Enemy;

Enemy::Enemy(
	const DGL::Math::Float2 spawnPosition,
	const DGL::Math::Float2 size,
	const int health,
	const float maxSpeed,
	const float maxForce
) :	Position{ spawnPosition },
	Velocity{ 0.0f, 0.0f },
	Acceleration{ 0.0f, 0.0f },
	Size{ size },
	MaxSpeed{ maxSpeed },
	MaxForce{ maxForce },
	Health{ health },
	PendingDamage{ 0 }
{
}

void Enemy::ApplyDamage(const DelayedDamage& damage)
{
	PendingDamage += damage.Amount;
	PendingDamages.push_back(damage);
}

bool Enemy::IsAboutToDie() const
{
	return (Health - PendingDamage) <= 0;
}

bool Enemy::IsDead() const
{
	return Health <= 0;
}

void Enemy::UpdatePendingDamages(const float deltaTime)
{
	for (size_t i = PendingDamages.size(); i-- > 0;)
	{
		DelayedDamage& damage = PendingDamages[i];
		damage.Delay -= deltaTime;

		if (damage.Delay <= 0.0f)
		{
			OnDamage(damage.Amount);
			Health -= damage.Amount;
			PendingDamage -= damage.Amount;

			PendingDamages.erase(PendingDamages.begin() + i);
		}
	}
}

void Enemy::Move(const float deltaTime)
{
	// Update the velocity
	Velocity = Velocity + Acceleration * deltaTime;
	Velocity = Velocity.Limited(MaxSpeed);

	// Move the object
	Position += Velocity * deltaTime;

	// Reset acceleration
	Acceleration = DGL::Math::Float2{ 0.0f, 0.0f };
}

void Enemy::ApplyForce(const DGL::Math::Float2 force)
{
	Acceleration += force;
}